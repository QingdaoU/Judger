#include <stdio.h>
#include "../src/runner.h"

int main() {
    runner_config config;
    config.max_cpu_time = 100;
    config.max_real_time = 100;
    config.max_memory = 1024 * 1024 * 1024;
    config.max_process_number = 1024;
    config.max_output_size = 1024 * 1024 * 64;
    config.log_path = "log.log";
    config.seccomp_rule_so_path = NULL;//"/home/virusdefender/ClionProjects/Judger/output/lib/librule_c_cpp.so";
    config.input_path = "/dev/null";
    config.output_path = "output.txt";
    config.error_path = "error.txt";
    config.exe_path = "/bin/ls";

    config.args[0] = "/bin/ls";
    config.args[1] = "/dev";
    config.args[2] = NULL;

    config.env[0] = "PATH=test";
    config.env[1] = NULL;

    runner_result result;
    run(&config, &result);

    printf("cpu time:%d\nreal time:%d\nmemory:%ld\nsignal:%d\nexit code:%d\nerror:%d", result.cpu_time, result.real_time, result.memory, result.signal, result.exit_code, result.error);
    return 0;
}
