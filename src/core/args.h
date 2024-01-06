#ifndef ARGS_H
#define ARGS_H

typedef enum {
    StartCommand,
    StopCommand,
    ReloadCommand,
    TestConfCommand,
} Command;

typedef struct {
    Command command;
    char* conf_file;
    int daemon_off;
} Args;

Args parse_args(int argc, char* argv[]);

#endif
