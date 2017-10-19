#ifndef JUDGER_LOGGER_H
#define JUDGER_LOGGER_H

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3


FILE *log_open(const char *);

void log_close(FILE *);

void log_write(int level, const char *source_filename, const int line_number, const FILE *log_fp, const char *, ...);

#ifdef JUDGER_DEBUG
#define LOG_DEBUG(log_fp, x...)   log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, log_fp, ##x)
#else
#define LOG_DEBUG(log_fp, x...)
#endif

#define LOG_INFO(log_fp, x...)  log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, log_fp, ##x)
#define LOG_WARNING(log_fp, x...) log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, log_fp, ##x)
#define LOG_FATAL(log_fp, x...)   log_write(LOG_LEVEL_FATAL, __FILE__, __LINE__, log_fp, ##x)

#endif //JUDGER_LOGGER_H
