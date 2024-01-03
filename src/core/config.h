#ifndef CONFIG_H
#define CONFIG_H

#define PORT_MAX 6
#define DIRECTIVE_MAX 100

typedef struct {
    int code;
    char* path;
} ErrorPage;

typedef struct {
    char* logfile;
    char* pidfile;
    char* port;
    char* root;
    char* index;
    ErrorPage* error_pages;
    int error_pages_len;
} Config;

typedef enum {
    CR_Success,
    CR_Failure,
} ConfigResult;

ConfigResult load_config(const char* conf_file);
void print_config();

extern Config CONFIG;

#endif
