#ifndef JUDGER_RUNNER_H
#define JUDGER_RUNNER_H
#endif


#define __DEBUG__


#ifdef __DEBUG__
#define print(format, ...) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define print(format,...)
#endif


#define RUN_SUCCEEDED 0
#define FORK_FAILED -1
#define WAIT4_FAILED -2
#define RUN_FAILED -3


#define SUCCESS 0
#define CPU_TIME_LIMIT_EXCEEDED 1
#define REAL_TIME_LIMIT_EXCEEDED 2
#define MEMORY_LIMIT_EXCEEDED 3
#define RUNTIME_ERROR 4
#define SYSTEM_ERROR 5


struct result {
    int cpu_time;
    long memory;
    int real_time;
    int signal;
    int flag;
    int error;
};


struct config {
    int max_cpu_time;
    int max_memory;
    char *path;
    char *in_file;
    char *out_file;
    char *args[100];
    char *env[100];
};


int run(struct config *, struct result *);

