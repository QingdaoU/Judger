#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/file.h>

#include "logger.h"

#define log_buffer_size 8192


FILE *log_open(const char *filename) {
    FILE *log_fp = fopen(filename, "a");
    if (log_fp == NULL) {
        fprintf(stderr, "can not open log file %s", filename);
    }
    return log_fp;
}


void log_close(FILE *log_fp) {
    if (log_fp != NULL) {
        fclose(log_fp);
    }
}


void log_write(int level, const char *source_filename, const int line, const FILE *log_fp, const char *fmt, ...) {
    char LOG_LEVEL_NOTE[][10] = {"FATAL", "WARNING", "INFO", "DEBUG"};
    if (log_fp == NULL) {
        fprintf(stderr, "can not open log file");
        return;
    }
    static char buffer[log_buffer_size];
    static char log_buffer[log_buffer_size];
    static char datetime[100];
    static char line_str[20];
    static time_t now;
    now = time(NULL);

    strftime(datetime, 99, "%Y-%m-%d %H:%M:%S", localtime(&now));
    snprintf(line_str, 19, "%d", line);
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_buffer, log_buffer_size, fmt, ap);
    va_end(ap);

    int count = snprintf(buffer, log_buffer_size,
                         "%s [%s] [%s:%s]%s\n",
                         LOG_LEVEL_NOTE[level], datetime, source_filename, line_str, log_buffer);
    // fprintf(stderr, "%s", buffer);
    int log_fd = fileno((FILE *) log_fp);
    if (flock(log_fd, LOCK_EX) == 0) {
        if (write(log_fd, buffer, (size_t) count) < 0) {
            fprintf(stderr, "write error");
            return;
        }
        flock(log_fd, LOCK_UN);
    }
    else {
        fprintf(stderr, "flock error");
        return;
    }
}