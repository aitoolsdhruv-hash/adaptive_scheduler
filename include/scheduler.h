#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_THREADS 8
#define MAX_QUEUE_SIZE 100
#define MAX_PROCESS_TYPE_LEN 50
#define MAX_AI_PROFILES 50
#define DEFAULT_SPLIT 2
#define BUFFER_SIZE 4096
#define OVERHEAD_CONSTANT 5.0f
#define BUCKET_SIZE 200

typedef enum {
    MODE_ADAPTIVE,
    MODE_NAIVE
} RunMode;

extern RunMode current_mode;

// OS Workload Classes
typedef enum {
    CLASS_IO_BOUND,
    CLASS_CPU_BOUND,
    CLASS_DIR_TRAVERSAL,
    CLASS_SYSTEM_MONITOR,
    CLASS_UNKNOWN
} WorkloadClass;

// AI Profile struct
typedef struct {
    WorkloadClass workload_class;
    int workload_bucket;
    float avg_time[MAX_THREADS + 1];
    int runs_per_thread[MAX_THREADS + 1];
    int optimal_threads;
    int total_runs;
} AIProfile;

// Subtask definition
typedef struct {
    int job_id;
    int subtask_id;
    int range_start;
    int range_end;
    char process_type[MAX_PROCESS_TYPE_LEN];
} Subtask;

// Job definition
typedef struct {
    int job_id;
    char process_type[MAX_PROCESS_TYPE_LEN];
    int workload_size;
    int num_subtasks;
    int subtasks_done;
    long start_time;
    long end_time;
    Subtask* subtasks;
} Job;

// WorkerThread definition
typedef struct {
    pthread_t thread;
    int thread_id;
    int is_busy;
    Subtask* current_task;
    pthread_cond_t cond;
} WorkerThread;

// External Mutexes and Condition Variables
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t queue_cond;

extern pthread_mutex_t pool_mutex;
extern pthread_cond_t dispatcher_cond;

extern pthread_mutex_t ai_mutex;
extern pthread_mutex_t jobs_mutex;

extern pthread_cond_t queue_full_cond;

extern WorkerThread pool[MAX_THREADS];

// Utility
long current_time_ms(void);

// Queue prototypes
void init_queue(void);
void enqueue_job(Job* job);
Job* dequeue_job(void);
int job_queue_empty(void);

// Thread pool prototypes
void init_thread_pool(void);
void execute_subtask(Subtask* s);
void merge_result(Subtask* s);

// Executor prototypes
void execute_file_copy(Subtask* s);
void execute_file_search(Subtask* s);
void execute_process_monitor(Subtask* s);

// Dispatcher prototypes
void start_dispatcher(void);
Job* find_job(int job_id);
int count_free_threads(void);

// Decomposer prototypes
Subtask* decompose(Job* job, int n);

// AI Model prototypes
AIProfile* ai_lookup_by_class(WorkloadClass wc, int size);
int ai_decide(const char* type, int size, int free_threads, RunMode mode);
void ai_update(const char* type, int size, int threads, float time);
void ai_print_stats(const char* type, int size);

// Classification helpers
WorkloadClass get_workload_class(const char* type);
const char* get_class_name(WorkloadClass wc);

// New hardening prototypes
void run_benchmark();
void run_stress_test();
void run_cli(int argc, char** argv);

#endif // SCHEDULER_H
