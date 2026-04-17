# PROJECT REPORT: ADAPTIVE TASK SCHEDULER (ATS)
**Course**: Operating Systems (CP)
**Version**: 2.0 (Hardened)

---

## 1. ABSTRACT
The Adaptive Task Scheduler (ATS) is a behavioral-oriented heuristic engine designed to optimize multi-threaded execution in a dynamic operating system environment. Unlike traditional schedulers that rely on static thread counts, ATS abstracts processes into **Workload Classes** (IO, CPU, Metadata) and utilizes a **Shared Intelligence** model. By observing real-time performance and applying a mathematical overhead penalty, the system autonomously converges on the optimal degree of parallelism for any given task family.

## 2. INTRODUCTION
Current operating systems often struggle with "Context Switch Storms" or "Disk Thrashing" caused by over-parallelization of tasks. The objective of this project was to build a scheduler that "learns" the hardware's physical limits. ATS implements a feedback loop that measures execution time against a moving average, ensuring that the system only scales up when the performance gain significantly outweighs the management cost.

## 3. SYSTEM ARCHITECTURE
The system is built on a modular **Producer-Consumer-Dispatcher** architecture.

### 3.1 Core Components
*   **Thread Pool (`thread_pool.c`)**: Manages 8 persistent worker threads that utilize conditional hibernation to save CPU cycles when idle.
*   **Shared Queue (`queue.c`)**: A thread-safe circular buffer that decouples the submission of jobs from their execution.
*   **The Dispatcher (`dispatcher.c`)**: The orchestrator that pulls jobs, consults the AI engine, and decomposes work into non-overlapping ranges.
*   **AI Heuristic Engine (`ai_model.c`)**: Maintains statistical profiles of hardware performance across different workload buckets.

### 3.2 Security and Hardening
During the Phase 2 hardening process, we implemented:
*   **Buffer Overflow Protection**: Replaced unsafe `sprintf` calls with `snprintf`.
*   **Logic Verification**: A standalone unit test suite (`unit_tests.c`) that validates mathematical range slicing.
*   **Diagnostics**: Detailed error reporting using `strerror(errno)` for system-level failures.

## 4. THE AI LOGIC: BEHAVIORAL GENERALIZATION
The core innovation of ATS is **Shared Intelligence**.
*   **Workload Classes**: Tasks are grouped into families like `CPU_BOUND` (Log Analysis) or `IO_BOUND` (Backup).
*   **Heuristic Scoring**: The AI uses the formula:  
    `Score = Time(avg) + (Thread_Count * 5.0ms)`  
    This ensures the scheduler rejects configurations that are "fast but inefficient" due to high overhead.
*   **Adaptive Exploration**: Every 2nd run, the system experiments by shifting the thread count by ±1. This allows the scheduler to adapt to shifting system loads (e.g., if another application starts consuming RAM).

## 5. EXPERIMENT RESULTS AND ANALYSIS
We performed a comprehensive audit comparing the **Adaptive Mode** against a **Naive 1-Thread Baseline**.

| Workload Class | Naive (1-Thread) | Adaptive (AI) | **Speedup Ratio** |
| :--- | :--- | :--- | :--- |
| **CPU_BOUND** | 908ms | 516ms | **1.76x** |
| **DIR_TRAVERSAL** | 925ms | 525ms | **1.76x** |
| **SYSTEM_MONITOR** | 923ms | 590ms | **1.56x** |
| **IO_BOUND** | 892ms | 725ms | **1.23x** |

**Analysis**: The highest gains (76%) were observed in CPU-bound tasks where thread-level parallelism can be fully exploited by the multi-core architecture. IO-bound tasks were correctly throttled by the AI to 2 threads to prevent disk contention.

## 6. CONCLUSION
The Adaptive Task Scheduler successfully demonstrates that behavioral-oriented learning is a viable strategy for OS resource management. By abstracting "what" a process is doing into "how" it behaves, the system achieves significant performance gains (up to 1.76x) while remaining resilient to hardware-level bottlenecks.

## 7. REFERENCES
*   POSIX Threads (pthreads) Documentation.
*   Operating Systems: Three Easy Pieces (Arpaci-Dusseau).
*   Project Technical Specifications: `PROJECT_DETAILS.md`.
