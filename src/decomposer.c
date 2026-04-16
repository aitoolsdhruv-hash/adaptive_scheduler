#include "../include/scheduler.h"

Subtask* decompose(Job* job, int n) {
    WorkloadClass wc = get_workload_class(job->process_type);
    
    // Natural parallel limits based on class
    int max_limit = MAX_THREADS;
    if (wc == CLASS_IO_BOUND) max_limit = 4; // Prevent disk thrashing
    
    if (n > max_limit) {
        printf("   [Decomposer] Capping threads from %d to %d for %s class\n", 
               n, max_limit, get_class_name(wc));
        n = max_limit;
    }

    Subtask* subtasks = malloc(n * sizeof(Subtask));
    int chunk = job->workload_size / n;
    
    for (int i = 0; i < n; i++) {
        subtasks[i].job_id      = job->job_id;
        subtasks[i].subtask_id  = i;
        subtasks[i].range_start = i * chunk;
        subtasks[i].range_end   = (i == n - 1)
                                  ? job->workload_size
                                  : (i + 1) * chunk;
        strcpy(subtasks[i].process_type, job->process_type);
    }
    
    job->num_subtasks = n;
    job->subtasks_done = 0;
    job->subtasks = subtasks;
    
    printf("[Decomposer] Split %s job %d into %d subtasks.\n", job->process_type, job->job_id, n);
    return subtasks;
}
