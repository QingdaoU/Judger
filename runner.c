#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <seccomp.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "runner.h"


int set_timer(int sec, int ms, int is_cpu_time) {
    struct itimerval time_val;
    time_val.it_interval.tv_sec = time_val.it_interval.tv_usec = 0;
    time_val.it_value.tv_sec = sec;
    time_val.it_value.tv_usec = ms * 1000;
    if (setitimer(is_cpu_time ? ITIMER_VIRTUAL : ITIMER_REAL, &time_val, NULL) == -1) {
        log("setitimer failed");
        return SETITIMER_FAILED;
    }
    return SUCCESS;
}


int run(struct config *config, struct result *result) {
    int status;
    struct rusage resource_usage;
    struct timeval start, end;
    struct rlimit memory_limit;
    int signal;
    int return_code;
    int i;
    int syscalls_whitelist[] = {SCMP_SYS(read), SCMP_SYS(write), SCMP_SYS(fstat),
                                SCMP_SYS(mmap), SCMP_SYS(mprotect), SCMP_SYS(munmap),
                                SCMP_SYS(brk), SCMP_SYS(access), SCMP_SYS(exit_group)};

    int syscalls_whitelist_length = sizeof(syscalls_whitelist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;

#ifdef __APPLE__
    log("Warning: setrlimit with RLIMIT_AS to limit memory usage will not work on OSX");
#endif

    gettimeofday(&start, NULL);

    memory_limit.rlim_cur = memory_limit.rlim_max = (rlim_t) (config->max_memory) * 2;

    pid_t pid = fork();

    if (pid < 0) {
        log("fork failed");
        result->flag = SYSTEM_ERROR;
        result->error = FORK_FAILED;
        return RUN_FAILED;
    }

    if (pid) {
        // parent process

        // on success, returns the process ID of the child whose state has changed;
        // On error, -1 is returned.
        if (wait4(pid, &status, 0, &resource_usage) == -1) {
            log("wait4 failed");
            result->flag = SYSTEM_ERROR;
            result->error = WAIT4_FAILED;
            return RUN_FAILED;
        }
        result->cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                  resource_usage.ru_utime.tv_usec / 1000 +
                                  resource_usage.ru_stime.tv_sec * 1000 +
                                  resource_usage.ru_stime.tv_usec / 1000);

        result->memory = resource_usage.ru_maxrss;

        // osx: ru_maxrss the maximum resident set size utilized (in bytes).
        // linux: ru_maxrss (since Linux 2.6.32)This  is  the  maximum  resident set size used (in kilobytes).
        // For RUSAGE_CHILDREN, this is the resident set size of the largest child,
        // not the maximum resident set size of the processtree.

#ifdef __linux__
        result->memory = result->memory * 1024;
#endif
        result->signal = 0;
        result->flag = result->error = SUCCESS;

        return_code = WEXITSTATUS(status);
        if (return_code) {
            log("Error child return code, return code: %d", return_code);
            result->flag = RUNTIME_ERROR;
            result->error = return_code;
            return RUN_FAILED;
        }

        if (WIFSIGNALED(status)) {
            signal = WTERMSIG(status);
            log("Signal %d\n", signal);
            result->signal = signal;
            if (signal == SIGALRM) {
                result->flag = REAL_TIME_LIMIT_EXCEEDED;
            }
            else if (signal == SIGVTALRM) {
                result->flag = CPU_TIME_LIMIT_EXCEEDED;
            }
            else if (signal == SIGSEGV) {
                if (result->memory > config->max_memory) {
                    result->flag = MEMORY_LIMIT_EXCEEDED;
                }
                else {
                    result->flag = RUNTIME_ERROR;
                }
            }
            else {
                result->flag = RUNTIME_ERROR;
            }
        }
        else {
            if (result->memory > config->max_memory) {
                result->flag = MEMORY_LIMIT_EXCEEDED;
            }
        }
        gettimeofday(&end, NULL);
        result->real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);
        return SUCCESS;
    }
    else {
        // child process
        log("I'm child process\n");
        // On success, these system calls return 0.
        // On error, -1 is returned, and errno is set appropriately.
        if (setrlimit(RLIMIT_AS, &memory_limit)) {
            log("setrlimit failed\n");
            return SETRLIMIT_FAILED;
        }
        // cpu time
        if (set_timer(config->max_cpu_time / 1000, config->max_cpu_time % 1000, 1)) {
            log("Set cpu time timer failed");
            return SETITIMER_FAILED;
        }
        // real time * 3
        if (set_timer(config->max_cpu_time / 1000 * 3, (config->max_cpu_time % 1000) * 3 % 1000, 0)) {
            log("Set real time timer failed");
            return SETITIMER_FAILED;
        }

        // read stdin from in file
        // On success, these system calls return the new descriptor. 
        // On error, -1 is returned, and errno is set appropriately.
        if (dup2(fileno(fopen(config->in_file, "r")), 0) == -1) {
            log("dup2 stdin failed");
            return DUP2_FAILED;
        }
        // write stdout to out file
        if (dup2(fileno(fopen(config->out_file, "w")), 1) == -1) {
            log("dup2 stdout failed");
            return DUP2_FAILED;
        }

        // load seccomp rules
        ctx = seccomp_init(SCMP_ACT_KILL);
        if (!ctx) {
            exit(LOAD_SECCOMP_FAILED);
        }
        for(i = 0; i < syscalls_whitelist_length; i++) {
            if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscalls_whitelist[i], 0)) {
                exit(LOAD_SECCOMP_FAILED);
            }
        }
        // add extra rule for execve
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, config->path));
        if (seccomp_load(ctx)) {
            exit(LOAD_SECCOMP_FAILED);
        }
        seccomp_release(ctx);

        execve(config->path, config->args, config->env);
        log("execve failed");
        return EXCEVE_FAILED;
    }
}
