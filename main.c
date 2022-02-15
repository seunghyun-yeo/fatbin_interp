#include <string.h>
#include "loader.h"


int main(int argc, char **argv, char **envp) {
    struct linux_binprm bprm;

    memset(&bprm, 0, sizeof(bprm));
    int ret = loader_exec(execfd, exec_path, target_argv, target_environ, regs, info, &bprm)
    return 0;
}
