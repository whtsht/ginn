#ifndef MASTER_SERVER_H
#define MASTER_SERVER_H

#include <pthread.h>

typedef struct {
    pthread_t* threads;
    int threads_count;
    pthread_t* new_threads;
    int new_threads_count;
} ThreadStore;

extern ThreadStore THREAD_STORE;
extern pthread_t MASTER;

void master_start();
void worker_start();

void init_threads();
void create_new_threads();
void kill_threads();
void swap_threads();

#endif
