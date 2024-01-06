#ifndef MASTER_SERVER_H
#define MASTER_SERVER_H

#include <pthread.h>

typedef struct {
    pthread_t* threads;
    int threads_count;
} ThreadStore;

void master_start();

#endif
