#include "../include/scheduler.h"
#include <unistd.h>

void assign_to_threads(Job* j, Subtask* subtasks, int n) {
    pthread_mutex_lock(&pool_mutex);
    int assigned = 0;
    
    for (int i = 0; i < MAX_THREADS && assigned < n; i++) {
        if (!pool[i].is_busy) {
            pool[i].is_busy = 1;
            pool[i].current_task = &subtasks[assigned];
            pthread_cond_signal(&pool[i].cond);
            assigned++;
        }
    }
    pthread_mutex_unlock(&pool_mutex);
}

int count_free_threads() {
    int count = 0;
    pthread_mutex_lock(&pool_mutex);
    for (int i = 0; i < MAX_THREADS; i++) {
        if (!pool[i].is_busy) count++;
    }
    pthread_mutex_unlock(&pool_mutex);
    return count;
}

extern int queue_count;

void* dispatcher_function(void* arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (queue_count == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        pthread_mutex_unlock(&queue_mutex);

        Job* job = dequeue_job();

        if (job == NULL) continue;
        
        if (strcmp(job->process_type, "SHUTDOWN") == 0) {
            break; // Used just to kill dispatcher thread gracefully
        }

        // Wait for free threads before deciding
        int free_threads = 0;
        while ((free_threads = count_free_threads()) == 0) {
            usleep(1000); 
        }
        
        int n = ai_decide(job->process_type, job->workload_size, free_threads, current_mode);
        printf("[Dispatcher] Job %d (%s) -> %s Mode suggested %d threads\n", 
               job->job_id, job->process_type, 
               (current_mode == MODE_ADAPTIVE ? "Adaptive" : "Naive"), n);

        job->start_time = current_time_ms();
        Subtask* subtasks = decompose(job, n);
        assign_to_threads(job, subtasks, n);
    }
    return NULL;
}

void start_dispatcher() {
    pthread_t dispatcher_thread;
    pthread_create(&dispatcher_thread, NULL, dispatcher_function, NULL);
    pthread_detach(dispatcher_thread);
}
