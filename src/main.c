#include <linux/limits.h>
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
        kill(pid, SIGTERM);
        remove_pid(CONFIG.pidfile);
    } else {
        puts("ginn is not running");
    }
}

void reload(char *conf_file) {
    check_permission();

    int pid;
    if ((pid = check_pid(CONFIG.pidfile))) {
        size_t size = PATH_MAX + 18;
        char buf[size];
        snprintf(buf, size, "cp %s /tmp/ginn.conf", conf_file);
        system(buf);
        kill(pid, SIGHUP);
    } else {
        puts("ginn is not running");
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
        case ReloadCommand: {
            reload(args.conf_file);
            break;
        }
        case TestConfCommand: {
            puts("Successfully loaded configuration file.\nConfiguration:");
            print_config();
            break;
        }
    }
}
