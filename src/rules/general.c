#include <stdio.h>
#include <seccomp.h>

#include "../runner.h"


int general_seccomp_rules(struct config *_config) {
    int syscalls_blacklist[] = {SCMP_SYS(socket), SCMP_SYS(clone),
                                SCMP_SYS(fork), SCMP_SYS(vfork),
                                SCMP_SYS(writev), SCMP_SYS(kill),
                                SCMP_SYS(chdir), SCMP_SYS(fchdir),
                                SCMP_SYS(rename), SCMP_SYS(chmod),
                                SCMP_SYS(fchmod), SCMP_SYS(chown),
                                SCMP_SYS(fchown), SCMP_SYS(lchown),
                                SCMP_SYS(setuid), SCMP_SYS(setgid),
                                SCMP_SYS(capset), SCMP_SYS(mount),
                                SCMP_SYS(sethostname)};
    int syscalls_blacklist_length = sizeof(syscalls_blacklist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;
    // load seccomp rules
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        return LOAD_SECCOMP_FAILED;
    }
    for (int i = 0; i < syscalls_blacklist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, syscalls_blacklist[i], 0) != 0) {
            return LOAD_SECCOMP_FAILED;
        }
    }
    // add extra rule for execve
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_NE, (scmp_datum_t)(_config->exe_path))) != 0) {
        return LOAD_SECCOMP_FAILED;
    }
    // only fd 0 1 2 are allowed
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_GT, 2)) != 0) {
        return LOAD_SECCOMP_FAILED;
    }
    if (seccomp_load(ctx) != 0) {
        return LOAD_SECCOMP_FAILED;
    }
    seccomp_release(ctx);
    return 0;
}