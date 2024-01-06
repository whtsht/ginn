#ifndef CORE_WORKER_INFO
#define CORE_WORKER_INFO

#include <pthread.h>

typedef struct {
    // 0 => not active
    // 1 => active
    int active;
    pthread_t thread_id;
    int listen_sokcet;
    int epoll_fd;
    // 0 => Running
    // 1 => Shutdown
    int is_shutdown;
} WorkerInfo;

void add_worker_info(pthread_t thread_id, int listen_sokcet, int epoll_fd);
void set_shutdown_worker_info(pthread_t thread_id);
WorkerInfo get_worker_info(pthread_t thread_id);
void remove_worker_info(pthread_t thread_id);

#endif
