#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>

#define DEBUG 1

#define FORK_FAILED -1
#define WAIT4_FAILED -2


#define SUCCESS 0
#define TIME_LIMIT_EXCEEDED 1
#define MEMORY_LIMIT_EXCEEDED 2
#define RUNTIME_ERROR 3
#define SYSTEM_ERROR 4


struct result {
    int cpu_time;
    int memory;
    int flag;
};


struct run {
    int max_cpu_time;
    int max_memory;
};


static void
set_timer(unsigned long sec, unsigned long ms, int is_cputime) {
    struct itimerval tval;
    tval.it_interval.tv_sec = tval.it_interval.tv_usec = 0;
    tval.it_value.tv_sec = sec;
    tval.it_value.tv_usec = ms * 1000;
    if (setitimer(is_cputime ? ITIMER_VIRTUAL : ITIMER_REAL, &tval, NULL) == -1)
#ifdef DEBUG
        printf("SET TIMER ERROR\n");
#endif
}


struct run runner;

int main() {
    int time_limit = 1000;
    // KB
    int memory_limit = 1000;

    int status;
    struct rusage resource_usage;
    struct result result;

    runner.max_cpu_time = 1000;
    runner.max_memory = 1000;

    pid_t pid = fork();
    if (pid < 0) {
#ifdef DEBUG
        printf("FORK FAILED");
#endif
        return FORK_FAILED;
    }
    if (pid > 0) {
        //parent process
#ifdef DEBUG
        printf("%s", "I'm parent process\n");
#endif
        if (wait4(pid, &status, 0, &resource_usage) == -1) {
#ifdef DEBUG
            printf("wait4 FAILED");
#endif
            return WAIT4_FAILED;
        }
        result.cpu_time = resource_usage.ru_utime.tv_sec * 1000 +
                          resource_usage.ru_utime.tv_usec / 1000 +
                          resource_usage.ru_stime.tv_sec * 1000 +
                          resource_usage.ru_stime.tv_usec / 1000;
        result.memory = resource_usage.ru_maxrss;
        result.flag = SUCCESS;

        if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            if(signal == SIGALRM || signal == SIGVTALRM){
                result.flag = TIME_LIMIT_EXCEEDED;
            }
            else if(signal == SIGSEGV){
                if (result.memory > runner.max_memory) {
                    result.flag = MEMORY_LIMIT_EXCEEDED;
                }
                else {
                    result.flag = RUNTIME_ERROR;
                }
            }
            else{
                result.flag = RUNTIME_ERROR;
            }
        }
#ifdef DEBUG
        printf("cpu time %d\nmemory %d\nflag %d", result.cpu_time, result.memory, result.flag);
#endif

    }
    else {
        //child process
#ifdef DEBUG
        printf("%s", "I'm child process\n");
#endif
        set_timer(0, 900, 1);
        int i = 900000000;
        while (i) {
            i = i - 1;
        }
    }

    return 0;
}