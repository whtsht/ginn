#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#include "../core/config.h"
#include "../util/logger.h"
#include "master.h"

static void sig_hup_handler() {
    if (pthread_self() == MASTER) {
        logging(LOG_DEBUG, "master: hup");
        load_config("/tmp/ginn.conf");

        create_new_threads();
        kill_threads();
        swap_threads();
    }
}

static void sig_term_handler() {
    if (pthread_self() == MASTER) {
        logging(LOG_DEBUG, "master: term");
        kill_threads();
        exit(EXIT_SUCCESS);
    } else {
        logging(LOG_DEBUG, "worker: term");
        pthread_exit(NULL);
    }
}

void init_signal_handler() {
    /* ignore unused signals */
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    struct sigaction sa1;
    sigaction(SIGHUP, (struct sigaction *)NULL, &sa1);
    sa1.sa_handler = sig_hup_handler;
    sa1.sa_flags = SA_NODEFER;
    sigaction(SIGHUP, &sa1, (struct sigaction *)NULL);

    struct sigaction sa2;
    sigaction(SIGTERM, (struct sigaction *)NULL, &sa2);
    sa2.sa_handler = sig_term_handler;
    sa2.sa_flags = SA_NODEFER;
    sigaction(SIGTERM, &sa2, (struct sigaction *)NULL);
}
