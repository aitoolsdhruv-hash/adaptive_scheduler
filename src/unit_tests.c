#include "../include/scheduler.h"
#include <assert.h>

void test_classification() {
    printf("[Test] Checking Workload Classification...\n");
    assert(get_workload_class("File Copy") == CLASS_IO_BOUND);
    assert(get_workload_class("Log Analysis") == CLASS_CPU_BOUND);
    assert(get_workload_class("File Search") == CLASS_DIR_TRAVERSAL);
    assert(get_workload_class("Process Monitor") == CLASS_SYSTEM_MONITOR);
    assert(get_workload_class("Random Unknown") == CLASS_UNKNOWN);
    printf("   -> PASS\n");
}

void test_decomposer() {
    printf("[Test] Checking Decomposer Range Logic...\n");
    Job j;
    j.job_id = 999;
    j.workload_size = 1000;
    strcpy(j.process_type, "Log Analysis");
    
    // Test even split
    Subtask* s1 = decompose(&j, 4);
    assert(j.num_subtasks == 4);
    if (!(s1[0].range_start == 0 && s1[0].range_end == 249)) {
        printf("   [DEBUG] Range 0: Expected 0-249, Got %d-%d\n", s1[0].range_start, s1[0].range_end);
    }
    assert(s1[0].range_start == 0 && s1[0].range_end == 249);
    assert(s1[1].range_start == 250 && s1[1].range_end == 499);
    assert(s1[2].range_start == 500 && s1[2].range_end == 749);
    assert(s1[3].range_start == 750 && s1[3].range_end == 999);
    free(s1);

    // Test uneven split / small remainder
    j.workload_size = 10;
    Subtask* s2 = decompose(&j, 3);
    // Subtasks should be 0-2 (3), 3-5 (3), 6-9 (4)
    assert(s2[0].range_start == 0);
    assert(s2[2].range_end == 9);
    free(s2);
    printf("   -> PASS\n");
}

void test_queue() {
    printf("[Test] Checking Queue Integrity...\n");
    init_queue();
    Job j1, j2;
    j1.job_id = 101;
    j2.job_id = 102;

    enqueue_job(&j1);
    enqueue_job(&j2);
    
    Job* out1 = dequeue_job();
    Job* out2 = dequeue_job();
    
    assert(out1->job_id == 101);
    assert(out2->job_id == 102);
    assert(job_queue_empty() == 1);
    printf("   -> PASS\n");
}

void test_ai_math() {
    printf("[Test] Checking AI Heuristic Scoring...\n");
    // Mocking a profile update
    // If 1 thread takes 100ms, and 2 threads take 96ms
    // Score(1T) = 100 + (1 * 5) = 105
    // Score(2T) = 96 + (2 * 5) = 106
    // AI should prefer 1T because the speedup (4ms) < overhead (5ms)
    
    ai_update("Log Analysis", 600, 1, 100.0f);
    ai_update("Log Analysis", 600, 2, 96.0f);
    
    // Result should be 1 thread
    int decision = ai_decide("Log Analysis", 600, 8, MODE_ADAPTIVE);
    assert(decision == 1);
    
    // Now make 2 threads MUCH faster (e.g., 80ms)
    // Score(2T) = 80 + 10 = 90
    ai_update("Log Analysis", 600, 2, 80.0f);
    decision = ai_decide("Log Analysis", 600, 8, MODE_ADAPTIVE);
    assert(decision == 2);
    
    printf("   -> PASS\n");
}

int main() {
    printf("============================================\n");
    printf("      SCHEDULER LOGIC VERIFICATION\n");
    printf("============================================\n");
    
    test_classification();
    test_decomposer();
    test_queue();
    test_ai_math();
    
    printf("\n[RESULT] ALL INTERNAL FUNCTIONS OPERATING CORRECTLY.\n");
    return 0;
}
