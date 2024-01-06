#ifndef MASTER_SERVER_H
#define MASTER_SERVER_H

#include <pthread.h>

extern pthread_t worker;

void master_start();

#endif
