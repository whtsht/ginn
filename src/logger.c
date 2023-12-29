#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE* LOG_FILE;
LogLevel LOG_LEVEL = LOG_INFO;

void init_logger(const char* logfile, LogLevel loglevel) {
    if (!(LOG_FILE = fopen(logfile, "a"))) {
        perror("Can't open the logfile");
        exit(EXIT_FAILURE);
    }

    setvbuf(LOG_FILE, NULL, _IOLBF, 0);
    LOG_LEVEL = loglevel;
}

static char* get_prefix(LogLevel loglevel) {
    switch (loglevel) {
        case LOG_INFO: {
            return "INFO    ";
        }
        case LOG_WARNING: {
            return "WARNING ";
        }
        case LOG_ERROR: {
            return "ERROR   ";
        }
    }
}

void logging_(LogLevel loglevel, const char* message) {
    if (!LOG_FILE || LOG_LEVEL > loglevel) return;

    char* prefix = get_prefix(loglevel);

    char s[1000];
    time_t t = time(NULL);
    struct tm* p = localtime(&t);
    strftime(s, sizeof s, "%D:%T", p);

    fprintf(LOG_FILE, "[%s] %s: %s\n", s, prefix, message);
}

void logging(LogLevel loglevel, const char* restrict format, ...) {
    char* str;
    va_list args;

    va_start(args, format);
    if (0 > vasprintf(&str, format, args)) str = NULL;
    va_end(args);

    if (str) {
        logging_(loglevel, str);
        free(str);
    } else {
        logging_(LOG_ERROR, "[system] failed to format string");
    }
}

void cleanup_logger() {
    if (LOG_FILE) fclose(LOG_FILE);
}
