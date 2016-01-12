#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "runner.h"


void set_timer(int sec, int ms, int is_cpu_time) {
    struct itimerval time_val;
    time_val.it_interval.tv_sec = time_val.it_interval.tv_usec = 0;
    time_val.it_value.tv_sec = sec;
    time_val.it_value.tv_usec = ms * 1000;
    if (setitimer(is_cpu_time ? ITIMER_VIRTUAL : ITIMER_REAL, &time_val, NULL) == -1) {
        print("settimer failed\n");
    }
}


int run(struct config *config, struct result *result) {
    int status;
    struct rusage resource_usage;
    struct timeval start, end;
    struct rlimit memory_limit;
    int signal;
    char *argv[] = {config->path, NULL};

#ifdef __APPLE__
    print("Warning: setrlimit will not work on OSX");
#endif

    gettimeofday(&start, NULL);

    memory_limit.rlim_cur = memory_limit.rlim_max = (rlim_t) (config->max_memory) * 2;

    pid_t pid = fork();

    if (pid < 0) {
        print("fork failed\n");
        result->flag = SYSTEM_ERROR;
        result->error = FORK_FAILED;
        return RUN_FAILED;
    }

    if (pid > 0) {
        //parent process
        print("I'm parent process\n");
        if (wait4(pid, &status, 0, &resource_usage) == -1) {
            print("wait4 failed\n");
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

        if (WIFSIGNALED(status)) {
            signal = WTERMSIG(status);
            print("Signal %d\n", signal);
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
        return RUN_SUCCEEDED;
    }
    else {
        //child process
        print("I'm child process\n");
        if (setrlimit(RLIMIT_AS, &memory_limit) != 0)
            print("setrlimit failed\n");
        // cpu time
        set_timer(config->max_cpu_time / 1000, config->max_cpu_time % 1000, 1);
        // real time * 3
        set_timer(config->max_cpu_time / 1000 * 3, (config->max_cpu_time % 1000) * 3 % 1000, 0);

        dup2(fileno(fopen(config->in_file, "r")), 0);
        dup2(fileno(fopen(config->out_file, "w")), 1);

        execve(config->path, argv, NULL);
        print("execve failed\n");
        return RUN_FAILED;
    }
}


int main() {
    struct config config;
    struct result result;
    int run_ret;

    config.max_cpu_time = 4300;
    config.max_memory = 180000000;

    config.path = (char *) malloc(200);
    config.in_file = (char *) malloc(200);
    config.out_file = (char *) malloc(200);

    strcpy(config.path, "/Users/virusdefender/Desktop/judger/limit");
    strcpy(config.in_file, "/Users/virusdefender/Desktop/judger/in");
    strcpy(config.out_file, "/Users/virusdefender/Desktop/judger/out");

    run_ret = run(&config, &result);

    if (run_ret) {
        print("Run failed\n");
        return RUN_FAILED;
    }

    print("cpu time %d\nreal time %d\nmemory %ld\nflag %d\nsignal %d\nerr %d\n",
          result.cpu_time, result.real_time, result.memory, result.flag, result.signal, result.error);

    return 0;
}
