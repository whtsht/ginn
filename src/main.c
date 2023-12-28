#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#include "daemonize.h"

FILE *fp;

void sig_int_handler() {
    fprintf(fp, "good bye :)\n");
    fclose(fp);
    exit(EXIT_SUCCESS);
}

int main() {
    daemonize();

    // ignore unused signals
    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    struct sigaction sa;
    sigaction(SIGINT, (struct sigaction *)NULL, &sa);
    sa.sa_handler = sig_int_handler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &sa, (struct sigaction *)NULL);

    fp = fopen("/tmp/test.log", "w");
    while (1) {
        // TODO: Insert daemon code here.
        fprintf(fp, "First daemon started\n");
        fflush(fp);
        sleep(1);
    }
}
