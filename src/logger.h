#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
} LogLevel;

void init_logger(const char* logfile, LogLevel loglevel);
void logging(LogLevel loglevel, const char* message);
void cleanup_logger();

#endif
