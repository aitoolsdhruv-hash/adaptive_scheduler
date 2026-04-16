#include "../include/scheduler.h"

void init_queue() {
    queue_head = 0;
    queue_tail = 0;
    queue_count = 0;
}

void enqueue_job(Job* job) {
    pthread_mutex_lock(&queue_mutex);
    
    // Hardening: Blocking queue on overflow
    while (queue_count >= MAX_QUEUE_SIZE) {
        printf("[Queue] FULL. Blocking producer for Job %d...\n", job->job_id);
        pthread_cond_wait(&queue_full_cond, &queue_mutex);
    }
    
    job_queue[queue_tail] = job;
    queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
    queue_count++;
    
    // Signal the dispatcher that a new job is available
    pthread_cond_signal(&queue_cond);
    
    pthread_mutex_unlock(&queue_mutex);
}

// Internal helper for dispatcher to pop safely
Job* dequeue_job() {
    Job* job = NULL;
    pthread_mutex_lock(&queue_mutex);
    
    if (queue_count > 0) {
        job = job_queue[queue_head];
        queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
        queue_count--;
        
        // Hardening: Signal producer that space is now available
        pthread_cond_signal(&queue_full_cond);
    }
    
    pthread_mutex_unlock(&queue_mutex);
    return job;
}

int job_queue_empty() {
    int empty = 1;
    pthread_mutex_lock(&queue_mutex);
    empty = (queue_count == 0);
    pthread_mutex_unlock(&queue_mutex);
    return empty;
}
