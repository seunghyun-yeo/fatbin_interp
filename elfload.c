//
// Created by seunghyun on 2022-02-14.
//
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include "loader.h"

int load_elf_binary(struct linux_binprm *bprm, struct image_info *info)
{
    struct image_info interp_info;
    struct elfhdr elf_ex;
    char *elf_interpreter = NULL;
    char *scratch;

    memset(&interp_info, 0, sizeof(interp_info));

    info->start_mmap = (abi_ulong)ELF_START_MMAP;

    load_elf_image(bprm->filename, bprm->fd, info,
                   &elf_interpreter, bprm->buf);

    /* ??? We need a copy of the elf header for passing to create_elf_tables.
       If we do nothing, we'll have overwritten this when we re-use bprm->buf
       when we load the interpreter.  */
    elf_ex = *(struct elfhdr *)bprm->buf;

    /* Do this so that we can load the interpreter, if need be.  We will
       change some of these later */
    bprm->p = setup_arg_pages(bprm, info);

    scratch = g_new0(char, TARGET_PAGE_SIZE);
    if (STACK_GROWS_DOWN) {
        bprm->p = copy_elf_strings(1, &bprm->filename, scratch,
                                   bprm->p, info->stack_limit);
        info->file_string = bprm->p;
        bprm->p = copy_elf_strings(bprm->envc, bprm->envp, scratch,
                                   bprm->p, info->stack_limit);
        info->env_strings = bprm->p;
        bprm->p = copy_elf_strings(bprm->argc, bprm->argv, scratch,
                                   bprm->p, info->stack_limit);
        info->arg_strings = bprm->p;
    } else {
        info->arg_strings = bprm->p;
        bprm->p = copy_elf_strings(bprm->argc, bprm->argv, scratch,
                                   bprm->p, info->stack_limit);
        info->env_strings = bprm->p;
        bprm->p = copy_elf_strings(bprm->envc, bprm->envp, scratch,
                                   bprm->p, info->stack_limit);
        info->file_string = bprm->p;
        bprm->p = copy_elf_strings(1, &bprm->filename, scratch,
                                   bprm->p, info->stack_limit);
    }

    g_free(scratch);

    if (!bprm->p) {
        fprintf(stderr, "%s: %s\n", bprm->filename, strerror(E2BIG));
        exit(-1);
    }

    if (elf_interpreter) {
        load_elf_interp(elf_interpreter, &interp_info, bprm->buf);

        /* If the program interpreter is one of these two, then assume
           an iBCS2 image.  Otherwise assume a native linux image.  */

        if (strcmp(elf_interpreter, "/usr/lib/libc.so.1") == 0
            || strcmp(elf_interpreter, "/usr/lib/ld.so.1") == 0) {
            info->personality = PER_SVR4;

            /* Why this, you ask???  Well SVr4 maps page 0 as read-only,
               and some applications "depend" upon this behavior.  Since
               we do not have the power to recompile these, we emulate
               the SVr4 behavior.  Sigh.  */
            target_mmap(0, qemu_host_page_size, PROT_READ | PROT_EXEC,
                        MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        }
#ifdef TARGET_MIPS
        info->interp_fp_abi = interp_info.fp_abi;
#endif
    }

    /*
     * TODO: load a vdso, which would also contain the signal trampolines.
     * Otherwise, allocate a private page to hold them.
     */
    if (TARGET_ARCH_HAS_SIGTRAMP_PAGE) {
        abi_long tramp_page = target_mmap(0, TARGET_PAGE_SIZE,
                                          PROT_READ | PROT_WRITE,
                                          MAP_PRIVATE | MAP_ANON, -1, 0);
        if (tramp_page == -1) {
            return -errno;
        }

        setup_sigtramp(tramp_page);
        target_mprotect(tramp_page, TARGET_PAGE_SIZE, PROT_READ | PROT_EXEC);
    }

    bprm->p = create_elf_tables(bprm->p, bprm->argc, bprm->envc, &elf_ex,
                                info, (elf_interpreter ? &interp_info : NULL));
    info->start_stack = bprm->p;

    /* If we have an interpreter, set that as the program's entry point.
       Copy the load_bias as well, to help PPC64 interpret the entry
       point as a function descriptor.  Do this after creating elf tables
       so that we copy the original program entry point into the AUXV.  */
    if (elf_interpreter) {
        info->load_bias = interp_info.load_bias;
        info->entry = interp_info.entry;
        g_free(elf_interpreter);
    }

#ifdef USE_ELF_CORE_DUMP
    bprm->core_dump = &elf_core_dump;
#endif

    /*
     * If we reserved extra space for brk, release it now.
     * The implementation of do_brk in syscalls.c expects to be able
     * to mmap pages in this space.
     */
    if (info->reserve_brk) {
        abi_ulong start_brk = HOST_PAGE_ALIGN(info->brk);
        abi_ulong end_brk = HOST_PAGE_ALIGN(info->brk + info->reserve_brk);
        target_munmap(start_brk, end_brk - start_brk);
    }

    return 0;
}