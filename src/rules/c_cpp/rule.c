#include <stdio.h>
#include <seccomp.h>

#include "../../runner.h"


int load_seccomp(void *dl_handler, runner_config *config) {
    int syscalls_whitelist[] = {SCMP_SYS(read), SCMP_SYS(fstat),
                                SCMP_SYS(mmap), SCMP_SYS(mprotect),
                                SCMP_SYS(munmap), SCMP_SYS(open),
                                SCMP_SYS(arch_prctl), SCMP_SYS(brk),
                                SCMP_SYS(access), SCMP_SYS(exit_group),
                                SCMP_SYS(close)};
    int syscalls_whitelist_length = sizeof(syscalls_whitelist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;
    // load seccomp rules
    ctx = seccomp_init(SCMP_ACT_KILL);
    if (!ctx) {
        return LOAD_SECCOMP_FAILED;
    }
    for (int i = 0; i < syscalls_whitelist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscalls_whitelist[i], 0) != 0) {
            return LOAD_SECCOMP_FAILED;
        }
    }
    // add extra rule for execve
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, config->exe_path)) != 0) {
        return LOAD_SECCOMP_FAILED;
    }
    // only fd 0 1 2 are allowed
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_LE, 2)) != 0) {
       return LOAD_SECCOMP_FAILED;
    }
    if (seccomp_load(ctx) != 0) {
        return LOAD_SECCOMP_FAILED;
    }
    seccomp_release(ctx);
    return 0;
}