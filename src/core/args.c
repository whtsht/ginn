#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util/streq.h"
#include "default.h"

void print_usage() {
    fprintf(
        stderr,
        "Usage: ginn <command> <option>*\n\nCommands:\n"
        "start\n    Start the server\n"
        "stop\n    Graceful shutdown of the server\n"
        "reload\n    Reload the configuration file and restart the server\n"
        "test\n    Check if configuration file is valid\n"
        "\nOptions:\n"
        "-c <filepath>\n    Load the configuration file located at <filepath>\n"
        "-daemon-off\n    Disable daemonizeation and start in the "
        "foreground\n");
}

Args parse_args(int argc, char **argv) {
    Args args = default_args();

    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    if (streq(argv[1], "start")) {
        args.command = StartCommand;
    } else if (streq(argv[1], "stop")) {
        args.command = StopCommand;
    } else if (streq(argv[1], "reload")) {
        args.command = ReloadCommand;
    } else if (streq(argv[1], "test")) {
        args.command = TestConfCommand;
    }

    if (argc >= 3 && strcmp(argv[2], "-c") == 0) {
        args.conf_file = argv[3];
    }

    if ((argc >= 3 && streq(argv[2], "-daemon-off")) ||
        (argc >= 5 && streq(argv[4], "-daemon-off"))) {
        args.daemon_off = 1;
    }

    return args;
}
