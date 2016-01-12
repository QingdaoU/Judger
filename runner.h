#ifndef JUDGER_RUNNER_H
#define JUDGER_RUNNER_H
#endif


struct result {
    int cpu_time;
    long memory;
    int real_time;
    int signal;
    int flag;
    int err;
};


struct config {
    int max_cpu_time;
    int max_memory;
    char path[200];
    char in_file[200];
    char out_file[200];
};


int run(struct config *, struct result *);

