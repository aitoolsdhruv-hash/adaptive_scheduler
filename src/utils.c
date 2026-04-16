#include "../include/scheduler.h"
#include <sys/time.h>

// Global Data
Job* active_jobs[1000] = {NULL};
WorkerThread pool[MAX_THREADS];
RunMode current_mode = MODE_ADAPTIVE;

// Global Synchronization Primitives
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_full_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dispatcher_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ai_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;

Job* job_queue[MAX_QUEUE_SIZE];
int queue_head = 0;
int queue_tail = 0;
int queue_count = 0;

long current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
