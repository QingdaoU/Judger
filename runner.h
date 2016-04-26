#ifndef JUDGER_RUNNER_H
#define JUDGER_RUNNER_H

#define CPU_TIME_UNLIMITED -1
#define MEMORY_UNLIMITED -1


#define SUCCESS 0

#define FORK_FAILED 1
#define WAIT4_FAILED 2
#define RUN_FAILED 3
#define SETITIMER_FAILED 4
#define SETRLIMIT_FAILED 5
#define DUP2_FAILED 6
#define EXCEVE_FAILED 7
#define LOAD_SECCOMP_FAILED 8
#define SET_UID_FAILED 9
#define SET_GID_FAILED 10


#define CPU_TIME_LIMIT_EXCEEDED 1
#define REAL_TIME_LIMIT_EXCEEDED 2
#define MEMORY_LIMIT_EXCEEDED 3
#define RUNTIME_ERROR 4
#define SYSTEM_ERROR 5


#define ERROR(code) LOG_FATAL("judger return error code: %d", code);raise(SIGUSR1)


struct result {
    int cpu_time;
    long memory;
    int real_time;
    int signal;
    int flag;
    int exit_status;
};


struct config {
    int max_cpu_time;
    long max_memory;
    char *path;
    char *in_file;
    char *out_file;
    char *args[100];
    char *env[100];
    int use_sandbox;
    int use_nobody;
    char *log_path;
};


void run(struct config *, struct result *);

#endif
