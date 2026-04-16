#include "../include/scheduler.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <getopt.h>

#include <getopt.h>

extern Job* active_jobs[1000];
int global_job_id = 1;

void print_menu() {
    printf("\n============================================\n");
    printf("      ADAPTIVE TASK SCHEDULER MENU\n");
    printf("============================================\n");
    printf("1-9. Run Individual Load (Various Classes)\n");
    printf("B.   Competitive Benchmark (Naive vs AI)\n");
    printf("S.   High-Concurrency Stress Test\n");
    printf("0.   Shutdown Environment & Exit\n");
    printf("--------------------------------------------\n");
    printf("Enter selection: ");
}

const char* get_process_name(int choice) {
    switch(choice) {
        case 1: return "File Copy";
        case 2: return "File Search";
        case 3: return "Process Monitor";
        case 4: return "Log Analysis";
        case 5: return "Data Compression";
        case 6: return "Disk Cleanup";
        case 7: return "Media Indexing";
        case 8: return "Backup";
        case 9: return "Process Monitor";
        default: return "Unknown";
    }
}

void process_rounds(const char* type, int workload, int rounds, RunMode mode) {
    current_mode = mode;
    printf(">>> Scale: %d | Mode: %s\n", workload, (mode == MODE_ADAPTIVE ? "ADAPTIVE" : "NAIVE"));
    for (int r = 1; r <= rounds; r++) {
        Job* job = malloc(sizeof(Job));
        job->job_id = global_job_id++;
        strcpy(job->process_type, type);
        job->workload_size = workload;
        job->subtasks_done = 0;
        job->num_subtasks = 0;
        
        active_jobs[job->job_id % 1000] = job;
        enqueue_job(job);
        
        while (job->subtasks_done < job->num_subtasks || job->num_subtasks == 0) {
            usleep(1000); 
        }
    }
}

void run_benchmark() {
    char type[50];
    int workload, rounds;
    printf("Enter Class/Type to Benchmark (e.g. CPU_BOUND): ");
    scanf("%s", type);
    printf("Enter workload size: ");
    scanf("%d", &workload);
    printf("Enter rounds for profiling: ");
    scanf("%d", &rounds);

    long start_naive = current_time_ms();
    process_rounds(type, workload, rounds, MODE_NAIVE);
    long end_naive = current_time_ms();

    long start_adaptive = current_time_ms();
    process_rounds(type, workload, rounds, MODE_ADAPTIVE);
    long end_adaptive = current_time_ms();

    float naive_total = (float)(end_naive - start_naive);
    float adaptive_total = (float)(end_adaptive - start_adaptive);
    
    printf("\n============================================\n");
    printf("      BENCHMARK REPORT: %s\n", type);
    printf("============================================\n");
    printf("Naive Mode (Fixed):    %.0fms\n", naive_total);
    printf("Adaptive Mode (AI):   %.0fms\n", adaptive_total);
    printf("Speedup Ratio:         %.2fx\n", naive_total / adaptive_total);
    printf("============================================\n");
}

void run_stress_test() {
    printf("\n>>> Starting High-Concurrency Stress Test (50 Jobs)...\n");
    long start = current_time_ms();
    
    for (int i = 0; i < 50; i++) {
        Job* job = malloc(sizeof(Job));
        job->job_id = global_job_id++;
        
        // Randomly assign workload class
        int r = rand() % 4;
        if (r == 0) strcpy(job->process_type, "Log Analysis");
        else if (r == 1) strcpy(job->process_type, "File Copy");
        else if (r == 2) strcpy(job->process_type, "Media Indexing");
        else strcpy(job->process_type, "Process Monitor");

        job->workload_size = 100 + (rand() % 900);
        job->subtasks_done = 0;
        job->num_subtasks = 0;
        active_jobs[job->job_id % 1000] = job;

        enqueue_job(job);
    }

    // Wait for all jobs to drain
    printf("[Stress] Jobs injected. Waiting for queue to drain...\n");
    while (1) {
        int all_done = 1;
        for (int i = 1; i < global_job_id; i++) {
            if (active_jobs[i % 1000] && active_jobs[i % 1000]->subtasks_done < active_jobs[i % 1000]->num_subtasks) {
                all_done = 0;
                break;
            }
        }
        if (all_done) break;
        usleep(100 * 1000);
    }
    
    long end = current_time_ms();
    printf("\n>>> STRESS TEST COMPLETE. Drained 50 jobs in %.0fms\n", (float)(end - start));
}

void run_cli(int argc, char** argv) {
    if (argc < 2) return;
    
    // Simple command-line logic
    if (strcmp(argv[1], "--stress") == 0) {
        run_stress_test();
    } else if (strcmp(argv[1], "--task") == 0 && argc >= 4) {
        process_rounds(argv[2], atoi(argv[3]), (argc > 4 ? atoi(argv[4]) : 1), MODE_ADAPTIVE);
        ai_print_stats(argv[2], atoi(argv[3]));
    }
    
    // Exit after CLI command
    exit(0);
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    
    init_queue();
    init_thread_pool();
    start_dispatcher();

#ifdef AUTO_STRESS
    run_stress_test();
    exit(0);
#endif

    if (argc > 1) {
        run_cli(argc, argv);
    }

#ifdef AUTO_DASHBOARD
    // Standard menu behavior
#endif

    char choice[10];
    while (1) {
        print_menu();
        scanf("%s", choice);

        if (choice[0] == '0') break;
        else if (choice[0] == 'B' || choice[0] == 'b') run_benchmark();
        else if (choice[0] == 'S' || choice[0] == 's') run_stress_test();
        else {
            int c = atoi(choice);
            if (c >= 1 && c <= 9) {
                int size, rounds;
                printf("Enter workload size: "); scanf("%d", &size);
                printf("Enter rounds: "); scanf("%d", &rounds);
                process_rounds(get_process_name(c), size, rounds, MODE_ADAPTIVE);
                ai_print_stats(get_process_name(c), size);
            }
        }
    }

    printf("\n[Main] Application terminated successfully.\n");
    return 0;
}
