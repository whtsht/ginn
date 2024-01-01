#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "default.h"

void print_usage() {
    fprintf(stderr,
            "Usage: ginn <command>\n\nCommands:\nstart\n    start web "
            "server\nstop\n    stop web server\n");
}

Args parse_args(int argc, char **argv) {
    Args args = default_args();

    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "start") == 0) {
        args.command = StartCommand;
    } else if (strcmp(argv[1], "stop") == 0) {
        args.command = StopCommand;
    } else if (strcmp(argv[1], "test") == 0) {
        args.command = TestConfCommand;
    }

    if (argc >= 4 && strcmp(argv[2], "-c") == 0) {
        args.conf_file = argv[3];
    }

    return args;
}
