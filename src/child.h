#ifndef JUDGER_CHILD_H
#define JUDGER_CHILD_H

#include <string.h>
#include "runner.h"

#define CHILD_ERROR_EXIT(error_code)\
    {\
        LOG_FATAL(log_fp, "Error: System errno: %s; Internal errno: "#error_code, strerror(errno)); \
        close_file(input_file); \
        if (output_file == error_file) { \
            close_file(output_file); \
        } else { \
            close_file(output_file); \
            close_file(error_file);  \
        } \
        raise(SIGUSR1);  \
        exit(EXIT_FAILURE); \
    }


void child_process(FILE *log_fp, struct config *_config);

#endif //JUDGER_CHILD_H
