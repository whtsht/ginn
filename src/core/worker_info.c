#include "worker_info.h"

#include <string.h>

#include "config.h"

#define MAX_LIST_SIZE (WORKER_PROCESSES_MAX * 2)

pthread_mutex_t mut;
WorkerInfo WORKER_INFO_LIST[MAX_LIST_SIZE];

void add_worker_info(pthread_t thread_id, int listen_sokcet, int epoll_fd) {
    pthread_mutex_lock(&mut);
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (!WORKER_INFO_LIST[i].active) {
            WORKER_INFO_LIST[i].active = 1;
            WORKER_INFO_LIST[i].thread_id = thread_id;
            WORKER_INFO_LIST[i].listen_sokcet = listen_sokcet;
            WORKER_INFO_LIST[i].epoll_fd = epoll_fd;
            WORKER_INFO_LIST[i].is_shutdown = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mut);
}

void set_shutdown_worker_info(pthread_t thread_id) {
    pthread_mutex_lock(&mut);
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (WORKER_INFO_LIST[i].thread_id == thread_id) {
            WORKER_INFO_LIST[i].is_shutdown = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mut);
}

WorkerInfo get_worker_info(pthread_t thread_id) {
    WorkerInfo worker_info;
    pthread_mutex_lock(&mut);
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (WORKER_INFO_LIST[i].thread_id == thread_id) {
            memcpy(&worker_info, &WORKER_INFO_LIST[i], sizeof(WorkerInfo));
            break;
        }
    }
    pthread_mutex_unlock(&mut);
    return worker_info;
}

void remove_worker_info(pthread_t thread_id) {
    pthread_mutex_lock(&mut);
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (WORKER_INFO_LIST[i].active &&
            WORKER_INFO_LIST[i].thread_id == thread_id) {
            WORKER_INFO_LIST[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mut);
}
