/*
 *
 * LOGGER v0.0.3
 * A simple logger for c/c++ under linux, multiprocess-safe
 *
 * ---- CopyLeft by Felix021 @ http://www.felix021.com ----
 *
 * LOG Format:
 * --LEVEL_NOTE--\x7 [Y-m-d H:m:s]\x7 [FILE:LINE]\x7 [EXTRA_INFO]\x7 log_info
 *   // LEVEL_NOTE stands for one of DEBUG/TRACE/NOTICE...
 *   // \x7 is a special character to separate logged fields.
 *
 * Usage:
 *   //Open log file first. Supply a log file name.
 *   log_open("log.txt");
 *
 *   //use it just as printf
 *   LOG_INFO("some info %d", 123);
 *
 *   //6 level: DEBUG, TRACE, NOTICE, MONITOR, WARNING, FATAL
 *   LOG_DEBUG("hi there");
 *
 *   //Need EXTRA_INFO to be logged automatically?
 *   log_add_info("pid:123");
 *
 *   //You don't need to call log_close manually, it'll be called at exit
 *   log_close();
 *
 */

#ifndef __LOGGER__
#define __LOGGER__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <error.h>
#include <sys/file.h>

int log_open(const char *filename);
void log_close();
static void log_write(int, const char *, const int, const char *, ...);
void log_add_info(const char *info);

const int LOG_FATAL         = 0;
const int LOG_WARNING       = 1;
const int LOG_INFO         = 2;
const int LOG_DEBUG         = 3;
static char LOG_LEVEL_NOTE[][10] =
        { "FATAL", "WARNING", "INFO", "DEBUG" };
#define LOG_DEBUG(x...)   log_write(LOG_DEBUG, __FILE__, __LINE__, ##x)
#define LOG_INFO(x...)  log_write(LOG_INFO, __FILE__ __LINE__, ##x)
#define LOG_WARNING(x...) log_write(LOG_WARNING, __FILE__, __LINE__, ##x)
#define LOG_FATAL(x...)   log_write(LOG_FATAL, __FILE__, __LINE__, ##x)


static FILE *log_fp                 = NULL;
static char *log_filename           = NULL;
static int  log_opened              = 0;

#define log_buffer_size 8192
static char log_buffer[log_buffer_size];
static char log_extra_info[log_buffer_size];

int log_open(const char* filename)
{
    if (log_opened == 1)
    {
        fprintf(stderr, "logger: log already opened\n");
        return 0;
    }
    int len = strlen(filename);
    log_filename = (char *)malloc(sizeof(char) * len + 1);
    strcpy(log_filename, filename);
    log_fp = fopen(log_filename, "a");
    if (log_fp == NULL)
    {
        fprintf(stderr, "log_file: %s", log_filename);
        perror("can't not open log file");
        exit(1);
    }

    atexit(log_close);
    log_opened = 1;
    log_extra_info[0] = 0;
    return 1;
}

void log_close()
{
    if (log_opened)
    {
        fclose(log_fp);
        free(log_filename);
        log_fp       = NULL;
        log_filename = NULL;
        log_opened   = 0;
    }
}

static void log_write(int level, const char *file,
                      const int line, const char *fmt, ...)
{
    if (log_opened == 0)
    {
        fprintf(stderr, "log_open not called yet\n");
        exit(1);
    }
    static char buffer[log_buffer_size];
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
                            "%s [%s] [%s:%d]%s %s\n",
                            LOG_LEVEL_NOTE[level], datetime, file, line, log_extra_info, log_buffer);
    fprintf(stdout, "%s", buffer);
    int log_fd = log_fp->_fileno;
    if (flock(log_fd, LOCK_EX) == 0)
    {
        if (write(log_fd, buffer, count) < 0)
        {
            perror("write error");
            exit(1);
        }
        flock(log_fd, LOCK_UN);
    }
    else
    {
        perror("flock error");
        exit(1);
    }
}

void log_add_info(const char *info)
{
    int len = strlen(log_extra_info);
    snprintf(log_extra_info + len, log_buffer_size - len, "\n [%s]", info);
}

#endif