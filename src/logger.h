#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
} LogLevel;

void init_logger(const char* logfile, LogLevel loglevel);
void logging_(LogLevel loglevel, const char* message);
void logging(LogLevel loglevel, const char* __restrict format, ...)
    __attribute__((format(printf, 2, 3)));
void cleanup_logger();

#endif
