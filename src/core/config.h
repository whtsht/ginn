#ifndef CONFIG_H
#define CONFIG_H

#define PORT_MAX 6
#define DIRECTIVE_MAX 100
#define WORKER_CONNECTION_MAX 10000
#define WORKER_CONNECTION_LENGTH 6
#define WORKER_PROCESSES_MAX 10000
#define WORKER_PROCESSES_LENGTH 6

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
    int worker_connections;
    int worker_processes;
} Config;

typedef enum {
    CR_Success,
    CR_Failure,
} ConfigResult;

ConfigResult load_config(const char* conf_file);
void print_config();
char* get_error_page(int error_code);

extern Config CONFIG;

#endif
