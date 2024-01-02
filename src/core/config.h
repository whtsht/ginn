#ifndef CONFIG_H
#define CONFIG_H

#define PORT_MAX 6
#define DIRECTIVE_MAX 100

typedef struct {
    char* logfile;
    char* pidfile;
    char* port;
} Config;

typedef enum {
    CR_Success,
    CR_Failure,
} ConfigResult;

ConfigResult load_config(const char* conf_file);
void print_config();

extern Config CONFIG;

#endif
