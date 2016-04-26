#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <seccomp.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "runner.h"
#include "logger.h"


int set_timer(int sec, int ms, int is_cpu_time) {
    struct itimerval time_val;
    time_val.it_interval.tv_sec = time_val.it_interval.tv_usec = 0;
    time_val.it_value.tv_sec = sec;
    time_val.it_value.tv_usec = ms * 1000;
    if (setitimer(is_cpu_time ? ITIMER_VIRTUAL : ITIMER_REAL, &time_val, NULL)) {
        LOG_FATAL("setitimer failed, errno %d", errno);
        return SETITIMER_FAILED;
    }
    return SUCCESS;
}


void run(struct config *config, struct result *result) {
    int status;
    struct rusage resource_usage;
    struct timeval start, end;
    struct rlimit memory_limit;
    int signal;
    int i;
    struct passwd *passwd = getpwnam("nobody");
    FILE *in_file, *out_file;
    int syscalls_whitelist[] = {SCMP_SYS(read), SCMP_SYS(fstat),
                                SCMP_SYS(mmap), SCMP_SYS(mprotect), 
                                SCMP_SYS(munmap), SCMP_SYS(open), 
                                SCMP_SYS(arch_prctl), SCMP_SYS(brk), 
                                SCMP_SYS(access), SCMP_SYS(exit_group), 
                                SCMP_SYS(close)};

    int syscalls_whitelist_length = sizeof(syscalls_whitelist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;
    
    log_open(config->log_path);

#ifdef __APPLE__
    #warning "setrlimit with RLIMIT_AS to limit memory usage will not work on OSX"
#endif

    gettimeofday(&start, NULL);

    if(config->max_memory < 1 && config->max_memory != MEMORY_UNLIMITED) {
        LOG_FATAL("max_memory must > 1 or unlimited");
        result->flag = SYSTEM_ERROR;
        return;
    }
    if(config->max_cpu_time < 1 && config->max_cpu_time != CPU_TIME_UNLIMITED) {
        LOG_FATAL("max_cpu_time must > 1 or unlimited");
        result->flag = SYSTEM_ERROR;
        return;
    }

    in_file = fopen(config->in_file, "r");
    out_file = fopen(config->out_file, "w");
    if(in_file == NULL || out_file == NULL) {
        LOG_FATAL("failed to open in/out redirect file");
        result->flag = SYSTEM_ERROR;
        return;
    }


    pid_t pid = fork();

    if (pid < 0) {
        LOG_FATAL("fork failed");
        result->flag = SYSTEM_ERROR;
        return;
    }

    if (pid > 0) {
        // parent process

        // on success, returns the process ID of the child whose state has changed;
        // On error, -1 is returned.
        if (wait4(pid, &status, 0, &resource_usage) == -1) {
            LOG_FATAL("wait4 failed");
            result->flag = SYSTEM_ERROR;
            return;
        }
        LOG_DEBUG("exit status: %d", WEXITSTATUS(status));
        result->exit_status = WEXITSTATUS(status);
        result->cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                  resource_usage.ru_utime.tv_usec / 1000 +
                                  resource_usage.ru_stime.tv_sec * 1000 +
                                  resource_usage.ru_stime.tv_usec / 1000);
         // avoid 0 ms
        if(result->cpu_time == 0) {
            result->cpu_time = 1;
        }

        // osx: ru_maxrss the maximum resident set size utilized (in bytes).
        // linux: ru_maxrss (since Linux 2.6.32)This  is  the  maximum  resident set size used (in kilobytes).
        // For RUSAGE_CHILDREN, this is the resident set size of the largest child,
        // not the maximum resident set size of the processtree.
        result->memory = resource_usage.ru_maxrss * 1024;

        result->signal = 0;
        result->flag = SUCCESS;

        if (WIFSIGNALED(status) != 0) {
            signal = WTERMSIG(status);
            LOG_DEBUG("signal: %d", signal);
            result->signal = signal;
            if (signal == SIGALRM) {
                result->flag = REAL_TIME_LIMIT_EXCEEDED;
            }
            else if (signal == SIGVTALRM) {
                result->flag = CPU_TIME_LIMIT_EXCEEDED;
            }
            else if (signal == SIGSEGV) {
                if (config->max_memory != MEMORY_UNLIMITED && result->memory > config->max_memory) {
                    result->flag = MEMORY_LIMIT_EXCEEDED;
                }
                else {
                    result->flag = RUNTIME_ERROR;
                }
            }
            // Child process error
            else if (signal == SIGUSR1){
                result->flag = SYSTEM_ERROR;
            }
            else {
                result->flag = RUNTIME_ERROR;
            }
        }
        else {
            if (config->max_memory != MEMORY_UNLIMITED && result->memory > config->max_memory) {
                result->flag = MEMORY_LIMIT_EXCEEDED;
            }
            if (WEXITSTATUS(status) != 0) {
                result->flag = RUNTIME_ERROR;
            }
        }
        gettimeofday(&end, NULL);
        result->real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);
    }
    else {
        // child process
        // On success, these system calls return 0.
        // On error, -1 is returned, and errno is set appropriately.
        if (config->max_memory != MEMORY_UNLIMITED) {
            memory_limit.rlim_cur = memory_limit.rlim_max = (rlim_t) (config->max_memory) * 2;
            if (setrlimit(RLIMIT_AS, &memory_limit) == -1) {
                LOG_FATAL("setrlimit failed, errno: %d", errno);
                ERROR(SETRLIMIT_FAILED);
            }
        }
        if (config->max_cpu_time != CPU_TIME_UNLIMITED) {
            // cpu time
            if (set_timer(config->max_cpu_time / 1000, config->max_cpu_time % 1000, 1) != SUCCESS) {
                LOG_FATAL("set cpu time timer failed");
                ERROR(SETITIMER_FAILED);
            }
            // real time * 3
            if (set_timer(config->max_cpu_time / 1000 * 3, (config->max_cpu_time % 1000) * 3 % 1000, 0) != SUCCESS) {
                LOG_FATAL("set real time timer failed");
                ERROR(SETITIMER_FAILED);
            }
        }

        // read stdin from in file
        // On success, these system calls return the new descriptor. 
        // On error, -1 is returned, and errno is set appropriately.
        if (dup2(fileno(in_file), 0) == -1) {
            LOG_FATAL("dup2 stdin failed, errno: %d", errno);
            ERROR(DUP2_FAILED);
        }
        // write stdout to out file
        if (dup2(fileno(out_file), 1) == -1) {
            LOG_FATAL("dup2 stdout failed, errno: %d", errno);
            ERROR(DUP2_FAILED);
        }
        // redirect stderr to stdout
        if (dup2(fileno(stdout), fileno(stderr)) == -1) {
            LOG_FATAL("dup2 stderr failed, errno: %d", errno);
            ERROR(DUP2_FAILED);
        }

        if (config->use_nobody != 0) {
            if(passwd == NULL) {
                LOG_FATAL("get nobody user info failed, errno: %d", errno);
                ERROR(SET_UID_FAILED);
            }
            if (setgid(passwd->pw_gid) == -1) {
                LOG_FATAL("setgid failed, errno: %d", errno);
                ERROR(SET_GID_FAILED);
            }
            if (setuid(passwd->pw_uid) == -1) {
                LOG_FATAL("setuid failed, errno: %d", errno);
                ERROR(SET_UID_FAILED);
            }
        }

        if (config->use_sandbox != 0) {
            // load seccomp rules
            ctx = seccomp_init(SCMP_ACT_KILL);
            if (!ctx) {
                LOG_FATAL("init seccomp failed");
                ERROR(LOAD_SECCOMP_FAILED);
            }
            for (i = 0; i < syscalls_whitelist_length; i++) {
                if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscalls_whitelist[i], 0) != 0) {
                    LOG_FATAL("load syscall white list failed");
                    ERROR(LOAD_SECCOMP_FAILED);
                }
            }
            // add extra rule for execve
            if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, config->path)) != 0) {
                LOG_FATAL("load execve rule failed");
                ERROR(LOAD_SECCOMP_FAILED);
            }
            // only fd 0 1 2 are allowed
            if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_LE, 2)) != 0) {
                LOG_FATAL("load dup2 rule failed");
                ERROR(LOAD_SECCOMP_FAILED);
            }
            if (seccomp_load(ctx) != 0) {
                LOG_FATAL("seccomp load failed");
                ERROR(LOAD_SECCOMP_FAILED);
            }
            seccomp_release(ctx);
        }
        execve(config->path, config->args, config->env);
        LOG_FATAL("execve failed, errno: %d", errno);
        ERROR(EXCEVE_FAILED);
    }
}
