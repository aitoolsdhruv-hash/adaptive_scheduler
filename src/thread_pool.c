#include "../include/scheduler.h"
#include <unistd.h>
#include <sys/time.h>

void execute_subtask(Subtask* s) {
    if (strcmp(s->process_type, "File Copy") == 0) {
        execute_file_copy(s);
    } else if (strcmp(s->process_type, "File Search") == 0) {
        execute_file_search(s);
    } else if (strcmp(s->process_type, "Process Monitor") == 0) {
        execute_process_monitor(s);
    } else {
        // Fallback simulate
        int size = s->range_end - s->range_start;
        usleep((size * 1000) / 10); 
    }
}

void merge_result(Subtask* s) {
    pthread_mutex_lock(&jobs_mutex);
    // In our simplified test, we just update via a global reference or pass it.
    // For Phase 2 we assume the parent Job pointer logic is handled in active_jobs
    Job* job = NULL;
    for(int i=0; i<1000; i++) {
        if(active_jobs[i] && active_jobs[i]->job_id == s->job_id) {
            job = active_jobs[i];
            break;
        }
    }
    
    if (job) {
        job->subtasks_done++;
        if (job->subtasks_done == job->num_subtasks) {
            job->end_time = current_time_ms();
            float elapsed = (float)(job->end_time - job->start_time);
            printf("[Result Merger] Job %d COMPLETE in %.0fms\n", job->job_id, elapsed);
            
            ai_update(job->process_type, job->workload_size, job->num_subtasks, elapsed);
            
            // Clean up subtasks
            free(job->subtasks);
        }
    }
    pthread_mutex_unlock(&jobs_mutex);
}

void* worker_function(void* arg) {
    WorkerThread* self = (WorkerThread*)arg;
    
    while (1) {
        pthread_mutex_lock(&pool_mutex);
        
        // Wait until there's a task assigned
        while (self->current_task == NULL) {
            pthread_cond_wait(&self->cond, &pool_mutex);
        }
        
        Subtask* task_to_run = self->current_task;
        pthread_mutex_unlock(&pool_mutex);
        
        // Perform the work outside of the pool lock
        execute_subtask(task_to_run);
        merge_result(task_to_run);
        
        // Reset state and notify dispatcher
        pthread_mutex_lock(&pool_mutex);
        self->is_busy = 0;
        self->current_task = NULL;
        pthread_cond_signal(&dispatcher_cond); // Wake dispatcher
        pthread_mutex_unlock(&pool_mutex);
    }
    return NULL;
}

void init_thread_pool() {
    for (int i = 0; i < MAX_THREADS; i++) {
        pool[i].thread_id = i;
        pool[i].is_busy = 0;
        pool[i].current_task = NULL;
        pthread_cond_init(&pool[i].cond, NULL);
        
        int status = pthread_create(&pool[i].thread, NULL, worker_function, &pool[i]);
        if (status != 0) {
            printf("[ERROR] Failed to start worker thread %d\n", i);
        }
    }
    printf("[Init] Thread pool of %d workers initialized.\n", MAX_THREADS);
}
