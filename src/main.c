#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "core/args.h"
#include "core/config.h"
#include "core/master.h"
#include "util/pidfile.h"

void check_permission() {
    if (!fopen(CONFIG.logfile, "a")) {
        perror("Can't access to logfile");
        exit(EXIT_FAILURE);
    }

    if (!fopen(CONFIG.pidfile, "a")) {
        perror("Can't access to pidfile");
        exit(EXIT_FAILURE);
    }
}

void stop() {
    check_permission();

    int pid;
    if ((pid = check_pid(CONFIG.pidfile))) {
        kill(pid, SIGINT);
        remove_pid(CONFIG.pidfile);
    } else {
        printf("not found %d\n", pid);
    }
}

int main(int argc, char *argv[]) {
    Args args = parse_args(argc, argv);
    if (load_config(args.conf_file)) {
        exit(EXIT_FAILURE);
    }

    switch (args.command) {
        case StartCommand: {
            master_start();
            break;
        }
        case StopCommand: {
            stop();
            break;
        }
        case TestConfCommand: {
            puts("Successfully loaded configuration file.\nConfiguration:");
            print_config();
            break;
        }
    }
}
