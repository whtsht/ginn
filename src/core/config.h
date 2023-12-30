#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char* logfile;
    char* pidfile;
    char* port;
} Config;

void load_config(const char* conf_file);

extern Config CONFIG;

#endif
