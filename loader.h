//
// Created by seunghyun on 2022-02-14.
//

#ifndef UNTITLED7_LOADER_H
#define UNTITLED7_LOADER_H
#define BPRM_BUF_SIZE 1024

typedef uint64_t abi_ulong __attribute__((aligned(8)));
struct linux_binprm{
    char buf[BPRM_BUF_SIZE] __attribute__((aligned));
    abi_ulong p;
    int fd;
    int e_uid, e_gid;
    int argc, envc;
    char **argv;
    char **envp;
    char *filename;
};

typedef uint32_t xtensa_reg_t;
typedef struct {} xtregs_opt_t;
struct target_pt_regs {
    xtensa_reg_t pc;            /*   4 */
    xtensa_reg_t ps;            /*   8 */
    xtensa_reg_t depc;          /*  12 */
    xtensa_reg_t exccause;      /*  16 */
    xtensa_reg_t excvaddr;      /*  20 */
    xtensa_reg_t debugcause;    /*  24 */
    xtensa_reg_t wmask;         /*  28 */
    xtensa_reg_t lbeg;          /*  32 */
    xtensa_reg_t lend;          /*  36 */
    xtensa_reg_t lcount;        /*  40 */
    xtensa_reg_t sar;           /*  44 */
    xtensa_reg_t windowbase;    /*  48 */
    xtensa_reg_t windowstart;   /*  52 */
    xtensa_reg_t syscall;       /*  56 */
    xtensa_reg_t icountlevel;   /*  60 */
    xtensa_reg_t scompare1;     /*  64 */
    xtensa_reg_t threadptr;     /*  68 */

    /* Additional configurable registers that are used by the compiler. */
    xtregs_opt_t xtregs_opt;

    /* Make sure the areg field is 16 bytes aligned. */
    int align[0] __attribute__ ((aligned(16)));

    /* current register frame.
     * Note: The ESF for kernel exceptions ends after 16 registers!
     */
    xtensa_reg_t areg[16];
};

struct image_info {
    abi_ulong       load_bias;
    abi_ulong       load_addr;
    abi_ulong       start_code;
    abi_ulong       end_code;
    abi_ulong       start_data;
    abi_ulong       end_data;
    abi_ulong       start_brk;
    abi_ulong       brk;
    abi_ulong       reserve_brk;
    abi_ulong       start_mmap;
    abi_ulong       start_stack;
    abi_ulong       stack_limit;
    abi_ulong       entry;
    abi_ulong       code_offset;
    abi_ulong       data_offset;
    abi_ulong       saved_auxv;
    abi_ulong       auxv_len;
    abi_ulong       arg_start;
    abi_ulong       arg_end;
    abi_ulong       arg_strings;
    abi_ulong       env_strings;
    abi_ulong       file_string;
    uint32_t        elf_flags;
    int             personality;
    abi_ulong       alignment;

    /* The fields below are used in FDPIC mode.  */
    abi_ulong       loadmap_addr;
    uint16_t        nsegs;
    void            *loadsegs;
    abi_ulong       pt_dynamic_addr;
    abi_ulong       interpreter_loadmap_addr;
    abi_ulong       interpreter_pt_dynamic_addr;
    struct image_info *other_info;

    /* For target-specific processing of NT_GNU_PROPERTY_TYPE_0. */
    uint32_t        note_flags;
};
#endif //UNTITLED7_LOADER_H
