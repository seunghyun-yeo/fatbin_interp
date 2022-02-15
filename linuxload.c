//
// Created by seunghyun on 2022-02-14.
//
#include "loader.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
static int count(char **vec){
    int i;
    for (i = 0; *vec, i++) {
        vec++;
    }
    return i;
}
static int prepare_binprm(struct linux_binprm *bprm){
    struct stat st;
    int mode, ret;
    if (fstat(bprm->fd, &stat) < 0) {
        return -errno;
    }
    mode = st.st_mode;
    if (!S_ISREG(mode)) {
        return -EACCESS;
    }
    if (!(mode & 0111)) {
        return -EACCESS;
    }

    bprm->e_uid = 0;
    bprm->e_gid = 0;

    if (mode & S_ISUID) {
        bprm->e_uid = st.st_uid;
    }
    if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP)) {
        bprm->e_gid = st.st_gid;
    }
    retval = read(bprm->fd, bprm->buf, BPRM_BUF_SIZE);
    if (retval < 0) {
        perror("prepare_binprm");
        exit(-1);
    }
    if (retval < BPRM_BUF_SIZE) {
        /* Make sure the rest of the loader won't read garbage.  */
        memset(bprm->buf + retval, 0, BPRM_BUF_SIZE - retval);
    }
    return retval;

}

int loader_exec(int fdexec, const char *filename, char **argv, char **envp, struct starget_pt_regs *regs,
                struct image_info *infop, struct linux_binprm *bprm)
{
    int ret;
    bprm->fd=fdexec;
    bprm->filename = (char *) filename;
    bprm->argc = count(argv);
    bprm->argv = argv;
    bprm->envc = count(envp);
    bprm->envp = envp;

    retval = prepare_binprm(bprm);
    if (retval >= 0) {
        if(bprm->buf[0] == 0x7f
        && bprm->buf[1] =='E'
        && bprm->buf[2] =='L'
        && bprm->buf[3] =='F'){
            retval = load_elf_binfary(bprm, infop);
        }
    }
}
