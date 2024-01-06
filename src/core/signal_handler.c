#include <signal.h>
#include <stdlib.h>

#include "../util/logger.h"

static void sig_int_handler() {
    logging(LOG_INFO, "good bye :)\n");
    cleanup_logger();
    exit(EXIT_SUCCESS);
}

void init_signal_handler() {
    /* ignore unused signals */
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    struct sigaction sa;
    sigaction(SIGINT, (struct sigaction *)NULL, &sa);
    sa.sa_handler = sig_int_handler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &sa, (struct sigaction *)NULL);
}
