#ifndef __LOGGER__
#define __LOGGER__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/file.h>

FILE* log_open(const char *);
void log_close(FILE*);
static void log_write(int level, const char* source_filename, const int line_number, const FILE* log_fp, const char*, ...);

const int LOG_FATAL         = 0;
const int LOG_WARNING       = 1;
const int LOG_INFO         = 2;
const int LOG_DEBUG         = 3;
static char LOG_LEVEL_NOTE[][10] =
        { "FATAL", "WARNING", "INFO", "DEBUG" };
#define LOG_DEBUG(log_fp, x...)   log_write(LOG_DEBUG, __FILE__, __LINE__, log_fp, ##x)
#define LOG_INFO(log_fp, x...)  log_write(LOG_INFO, __FILE__ __LINE__, log_fp, ##x)
#define LOG_WARNING(log_fp, x...) log_write(LOG_WARNING, __FILE__, __LINE__, log_fp, ##x)
#define LOG_FATAL(log_fp, x...)   log_write(LOG_FATAL, __FILE__, __LINE__, log_fp, ##x)

#define log_buffer_size 8192

FILE* log_open(const char* filename)
{
    FILE* log_fp = fopen(filename, "a");
    if (log_fp == NULL)
    {
        fprintf(stderr, "can not open log file %s", filename);
    }
    return log_fp;
}

void log_close(FILE* log_fp)
{
    if (log_fp != NULL)
    {
        fclose(log_fp);
    }
}

static void log_write(int level, const char* source_filename, const int line, const FILE* log_fp, const char *fmt, ...)
{
    if (log_fp == NULL)
    {
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

    size_t count = snprintf(buffer, log_buffer_size,
                            "%s [%s] [%s:%d]%s\n",
                            LOG_LEVEL_NOTE[level], datetime, source_filename, line, log_buffer);
    fprintf(stdout, "%s", buffer);
    int log_fd = fileno((FILE *)log_fp);
    if (flock(log_fd, LOCK_EX) == 0)
    {
        if (write(log_fd, buffer, count) < 0)
        {
            fprintf(stderr, "write error");
            return;
        }
        flock(log_fd, LOCK_UN);
    }
    else
    {
        fprintf(stderr, "flock error");
        return;
    }
}

#endif
