#include "master.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util/daemonize.h"
#include "../util/logger.h"
#include "../util/pidfile.h"
#include "config.h"
#include "signal_handler.h"
#include "worker.h"

void check_permission();

ThreadStore THREAD_STORE = {};

pthread_t MASTER;

void worker_start() {
    logging(LOG_DEBUG, "worker: start");
    int soc = server_socket(CONFIG.port);
    if (soc == -1) {
        logging(LOG_ERROR, "server_socket(%s): error\n", CONFIG.port);
    }

    accept_loop(soc);
    close(soc);
}

void init_threads() {
    THREAD_STORE.threads_count = CONFIG.worker_processes;
    THREAD_STORE.threads =
        malloc(sizeof(pthread_t) * THREAD_STORE.threads_count);

    for (int i = 0; i < THREAD_STORE.threads_count; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, (void *)worker_start, NULL);
        THREAD_STORE.threads[i] = thread;
    }
}

void create_new_threads() {
    THREAD_STORE.new_threads_count = CONFIG.worker_processes;
    THREAD_STORE.new_threads =
        malloc(sizeof(pthread_t) * THREAD_STORE.new_threads_count);

    for (int i = 0; i < THREAD_STORE.new_threads_count; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, (void *)worker_start, NULL);
        THREAD_STORE.new_threads[i] = thread;
    }
}

void kill_threads() {
    for (int i = 0; i < THREAD_STORE.threads_count; i++) {
        pthread_kill(THREAD_STORE.threads[i], SIGTERM);
    }

    for (int i = 0; i < THREAD_STORE.threads_count; i++) {
        pthread_join(THREAD_STORE.threads[i], NULL);
    }
}

void swap_threads() {
    free(THREAD_STORE.threads);
    THREAD_STORE.threads_count = THREAD_STORE.new_threads_count;
    THREAD_STORE.threads =
        malloc(sizeof(pthread_t) * THREAD_STORE.threads_count);
    memcpy(THREAD_STORE.threads, THREAD_STORE.new_threads,
           sizeof(pthread_t) * THREAD_STORE.threads_count);
    free(THREAD_STORE.new_threads);
}

void master_start(int daemon_off) {
    check_permission();

    int pid;
    if ((pid = check_pid(CONFIG.pidfile))) {
        printf("already exist\n");
        exit(EXIT_FAILURE);
    }

    if (!daemon_off) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid > 0) exit(EXIT_SUCCESS);

        daemonize();
    }

    init_signal_handler();

    init_logger(CONFIG.logfile, LOG_DEBUG);

    int ret = write_pid(CONFIG.pidfile);
    if (ret == 0) {
        logging(LOG_ERROR, "write_pid\n");
        exit(EXIT_FAILURE);
    }

    MASTER = pthread_self();

    init_threads();

    for (;;) {
        sleep(10);
    }
}
