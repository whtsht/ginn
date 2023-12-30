#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "args.h"
#include "config.h"
#include "daemonize.h"
#include "logger.h"
#include "pidfile.h"
#include "server.h"
#include "signal_handler.h"

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

void start() {
    check_permission();

    int pid;
    if ((pid = check_pid(CONFIG.pidfile))) {
        printf("already exist\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) exit(EXIT_SUCCESS);

    daemonize();

    init_signal_handler();

    init_logger(CONFIG.logfile, LOG_DEBUG);

    int ret = write_pid(CONFIG.pidfile);
    if (ret == 0) {
        logging(LOG_ERROR, "write_pid\n");
        exit(EXIT_FAILURE);
    }

    char* portnm = "8080";
    int soc = server_socket(portnm);
    if (soc == -1) {
        logging(LOG_ERROR, "server_socket(%s): error\n", portnm);
    }

    accept_loop(soc);
    close(soc);
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

int main(int argc, char* argv[]) {
    Args args = parse_args(argc, argv);
    load_config(args.conf_file);

    switch (args.command) {
        case StartCommand: {
            start();
            break;
        }
        case StopCommand: {
            stop();
            break;
        }
    }
}
