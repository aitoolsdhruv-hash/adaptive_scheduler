#include "../include/scheduler.h"

AIProfile profiles[MAX_AI_PROFILES];
int num_profiles = 0;
pthread_mutex_t ai_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MIN_THREADS(a, b) ((a) < (b) ? (a) : (b))

// Classification Logic
WorkloadClass get_workload_class(const char* type) {
    if (strcmp(type, "File Copy") == 0 || strcmp(type, "Backup") == 0) 
        return CLASS_IO_BOUND;
    if (strcmp(type, "Log Analysis") == 0 || strcmp(type, "Data Compression") == 0) 
        return CLASS_CPU_BOUND;
    if (strcmp(type, "File Search") == 0 || strcmp(type, "Disk Cleanup") == 0 || strcmp(type, "Media Indexing") == 0) 
        return CLASS_DIR_TRAVERSAL;
    if (strcmp(type, "Process Monitor") == 0) 
        return CLASS_SYSTEM_MONITOR;
    return CLASS_UNKNOWN;
}

const char* get_class_name(WorkloadClass wc) {
    switch(wc) {
        case CLASS_IO_BOUND:       return "IO_BOUND";
        case CLASS_CPU_BOUND:      return "CPU_BOUND";
        case CLASS_DIR_TRAVERSAL:  return "DIR_TRAVERSAL";
        case CLASS_SYSTEM_MONITOR: return "SYSTEM_MONITOR";
        default:                   return "UNKNOWN";
    }
}

AIProfile* ai_create_profile(WorkloadClass wc, int bucket) {
    if (num_profiles >= MAX_AI_PROFILES) return NULL;
    AIProfile* p = &profiles[num_profiles++];
    p->workload_class = wc;
    p->workload_bucket = bucket;
    
    for (int i = 0; i <= MAX_THREADS; i++) {
        p->avg_time[i] = 0.0f;
        p->runs_per_thread[i] = 0;
    }
    p->optimal_threads = DEFAULT_SPLIT;
    p->total_runs = 0;
    return p;
}

AIProfile* ai_lookup_by_class(WorkloadClass wc, int size) {
    int bucket = size / BUCKET_SIZE;
    for (int i = 0; i < num_profiles; i++) {
        if (profiles[i].workload_class == wc &&
            profiles[i].workload_bucket == bucket) {
            return &profiles[i];
        }
    }
    return NULL;
}

int ai_decide(const char* type, int size, int free_threads, RunMode mode) {
    if (mode == MODE_NAIVE) return MIN_THREADS(free_threads, DEFAULT_SPLIT);

    int decision = 1;
    WorkloadClass wc = get_workload_class(type);
    
    pthread_mutex_lock(&ai_mutex);
    AIProfile* p = ai_lookup_by_class(wc, size);
    
    if (p == NULL) {
        decision = MIN_THREADS(free_threads, DEFAULT_SPLIT);
    } else {
        // Every 4th run, do controlled exploration (adjacent threads)
        if (p->total_runs > 0 && p->total_runs % 4 == 0) {
            // Alternating exploration: try N+1 or N-1
            int shift = (p->total_runs % 8 == 0) ? 1 : -1;
            int explore = p->optimal_threads + shift;
            
            // Safety clamping
            if (explore < 1) explore = 1;
            if (explore > MAX_THREADS) explore = MAX_THREADS;
            
            decision = MIN_THREADS(explore, free_threads);
            printf("   [AI] Class %s Exploration! Trying %d threads (Shift: %d)\n", get_class_name(wc), decision, shift);
        } else {
            decision = MIN_THREADS(p->optimal_threads, free_threads);
        }
    }
    
    pthread_mutex_unlock(&ai_mutex);
    if (decision <= 0) return 1;
    return decision;
}

void ai_update(const char* type, int size, int threads, float time) {
    WorkloadClass wc = get_workload_class(type);
    pthread_mutex_lock(&ai_mutex);
    int bucket = size / BUCKET_SIZE;
    AIProfile* p = ai_lookup_by_class(wc, size);
    
    if (p == NULL) {
        p = ai_create_profile(wc, bucket);
    }
    
    if (p != NULL) {
        float old_avg = p->avg_time[threads];
        int n = p->runs_per_thread[threads];
        p->avg_time[threads] = ((old_avg * n) + time) / (n + 1);
        p->runs_per_thread[threads]++;
        p->total_runs++;

        int best_n = p->optimal_threads;
        float best_score = -1.0f;

        for (int i = 1; i <= MAX_THREADS; i++) {
            if (p->runs_per_thread[i] > 0) {
                float score = p->avg_time[i] + (i * OVERHEAD_CONSTANT);
                if (best_score < 0 || score < best_score) {
                    best_score = score;
                    best_n = i;
                }
            }
        }

        if (best_n != p->optimal_threads) {
            printf("   [AI UPDATE] Behavioral shift for %s! New optimal: %d threads\n", 
                   get_class_name(wc), best_n);
            p->optimal_threads = best_n;
        }
    }
    pthread_mutex_unlock(&ai_mutex);
}

void ai_print_stats(const char* type, int size) {
    WorkloadClass wc = get_workload_class(type);
    pthread_mutex_lock(&ai_mutex);
    AIProfile* p = ai_lookup_by_class(wc, size);
    if (p) {
        printf("   [AI Class Stats] Class: %s | Bucket: %d | Optimal: %d\n", 
               get_class_name(wc), p->workload_bucket, p->optimal_threads);
        printf("   [AI Metrics] ");
        for (int i = 1; i <= MAX_THREADS; i++) {
            if (p->runs_per_thread[i] > 0) {
                printf("%dT: %.1fms ", i, p->avg_time[i]);
            }
        }
        printf("\n");
    }
    pthread_mutex_unlock(&ai_mutex);
}
