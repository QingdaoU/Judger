#define _GNU_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <pthread.h>
#include <wait.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

#include "runner.h"
#include "killer.h"
#include "child.h"
#include "logger.h"

void init_result(struct result *_result) {
    _result->result = _result->error = SUCCESS;
    _result->cpu_time = _result->real_time = _result->signal = _result->exit_code = 0;
    _result->memory = 0;
}


void run(struct config *_config, struct result *_result) {
    // init log fp
    FILE *log_fp = log_open(_config->log_path);

    long pipe_memory;
    int pipes[2];

    int status;
    struct rusage child_resource_usage;

    struct timeval start, end;

    // init result
    init_result(_result);

    // check whether current user is root
    uid_t uid = getuid();
    if (uid != 0) {
        ERROR_EXIT(ROOT_REQUIRED);
    }

    // check args
    if ((_config->max_cpu_time < 1 && _config->max_cpu_time != UNLIMITED) ||
        (_config->max_real_time < 1 && _config->max_real_time != UNLIMITED) ||
        (_config->max_stack < 1) ||
        (_config->max_memory < 1 && _config->max_memory != UNLIMITED) ||
        (_config->max_process_number < 1 && _config->max_process_number != UNLIMITED) ||
        (_config->max_output_size < 1 && _config->max_output_size != UNLIMITED)) {
        ERROR_EXIT(INVALID_CONFIG);
    }

    if (pipe(pipes) != 0) {
        ERROR_EXIT(PIPE_FAILED);
    }

    // record current time
    gettimeofday(&start, NULL);

    pid_t child_pid = fork();

    // pid < 0 shows clone failed
    if (child_pid < 0) {
        ERROR_EXIT(FORK_FAILED);
    }
    else if (child_pid == 0) {
        child_process(log_fp, _config, pipes[1], &pipe_memory);
    }
    else if (child_pid > 0){
        // create new thread to monitor process running time
        pthread_t tid = 0;
        if (_config->max_real_time != UNLIMITED) {
            struct timeout_killer_args killer_args;

            killer_args.timeout = _config->max_real_time;
            killer_args.pid = child_pid;
            if (pthread_create(&tid, NULL, timeout_killer, (void *) (&killer_args)) != 0) {
                kill_pid(child_pid);
                ERROR_EXIT(PTHREAD_FAILED);
            }
        }

        // wait for child process to terminate
        // on success, returns the process ID of the child whose state has changed;
        // On error, -1 is returned.
        if (wait4(child_pid, &status, WSTOPPED, &child_resource_usage) == -1) {
            kill_pid(child_pid);
            ERROR_EXIT(WAIT_FAILED);
        }

        if (read(pipes[0], &pipe_memory, sizeof(pipe_memory)) < 0) {
            ERROR_EXIT(PIPE_FAILED);
        }

        // process exited, we may need to cancel timeout killer thread
        if (_config->max_real_time != UNLIMITED) {
            if (pthread_cancel(tid) != 0) {
                LOG_WARNING(log_fp, "cancel thread failed");
            };
        }

        _result->exit_code = WEXITSTATUS(status);
        _result->cpu_time = (int) (child_resource_usage.ru_utime.tv_sec * 1000 +
                                  child_resource_usage.ru_utime.tv_usec / 1000 +
                                  child_resource_usage.ru_stime.tv_sec * 1000 +
                                  child_resource_usage.ru_stime.tv_usec / 1000);
        long rss_delta = child_resource_usage.ru_maxrss - pipe_memory;
        if (rss_delta > 0) {
            _result->memory = rss_delta * 1024;
        }
        else {
            LOG_WARNING(log_fp, "child max rss < pipe memory, fall back to use child max rss");
            _result->memory = child_resource_usage.ru_maxrss * 1024;
        }


        // get end time
        gettimeofday(&end, NULL);
        _result->real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);

        if (_result->exit_code != 0) {
            _result->result = RUNTIME_ERROR;
        }
        // if signaled
        if (WIFSIGNALED(status) != 0) {
            LOG_DEBUG(log_fp, "signal: %d", WTERMSIG(status));
            _result->signal = WTERMSIG(status);
            if (_result->signal == SIGSEGV) {
                if (_config->max_memory != UNLIMITED && _result->memory > _config->max_memory) {
                    _result->result = MEMORY_LIMIT_EXCEEDED;
                }
                else {
                    _result->result = RUNTIME_ERROR;
                }
            }
            else if(_result->signal == SIGUSR1) {
                _result->result = SYSTEM_ERROR;
            }
            else {
                _result->result = RUNTIME_ERROR;
            }
        }
        else {
            if (_config->max_memory != UNLIMITED && _result->memory > _config->max_memory) {
                _result->result = MEMORY_LIMIT_EXCEEDED;
            }
        }
        if (_config->max_real_time != UNLIMITED && _result->real_time > _config->max_real_time) {
            _result->result = REAL_TIME_LIMIT_EXCEEDED;
        }
        if (_config->max_cpu_time != UNLIMITED && _result->cpu_time > _config->max_cpu_time) {
            _result->result = CPU_TIME_LIMIT_EXCEEDED;
        }

        log_close(log_fp);
    }
}
