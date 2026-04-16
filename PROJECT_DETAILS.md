# Adaptive Task Scheduler: Technical Deep Dive

This document provides a comprehensive explanation of the architecture, logic, and implementation of the Adaptive Task Scheduler.

## 1. Project Concept
The core idea is a **Behavioral-Oriented Adaptive Scheduler**. Instead of learning thread counts for specific filenames, the system abstracts processes into representative **OS Workload Classes** (e.g., IO-Bound, CPU-Bound). This allows for **Shared Intelligence**, where insights learned from one task automatically benefit other tasks with similar behavioral profiles.

## 2. System Architecture
The system follows a modular **Producer-Consumer-Dispatcher** model.

### 2.1 The Components
- **Job Queue (`src/queue.c`)**: A thread-safe circular buffer that decouples task arrival from execution.
- **Thread Pool (`src/thread_pool.c`)**: Spawns 8 persistent worker threads at startup that wait on condition variables (conditional hibernation).
- **The Dispatcher (`src/dispatcher.c`)**: Orchestrates the workflow by pulling jobs, consulting the AI, and decomposing tasks.
- **The Decomposer (`src/decomposer.c`)**: Partitions workloads into discrete ranges. It is **Class-Aware**, meaning it enforces natural parallel limits (e.g., capping IO-Bound tasks to 4 threads to prevent disk thrashing).
- **The AI Model (`src/ai_model.c`)**: Implements an **Adaptive Heuristic Learning** engine. It maps incoming process names to `WorkloadClasses` and manages statistical profiles based on those behaviors.
- **The Executor (`src/executor.c`)**: Interfaces with the OS hardware using POSIX system calls (I/O, directory traversal).

## 3. The "AI" Logic: Behavioral Generalization
The scheduler uses a sophisticated feedback mechanism that generalizes learning across the system.

### 3.1 Workload Class Mapping
Processes are categorized into behavioral "families":
- `CLASS_IO_BOUND`: High disk usage, low CPU (e.g., File Copy, Backup).
- `CLASS_CPU_BOUND`: High calculation, low I/O (e.g., Log Analysis, Data Compression).
- `CLASS_DIR_TRAVERSAL`: High metadata/directory operations (e.g., File Search, Disk Cleanup, Media Indexing).
- `CLASS_SYSTEM_MONITOR`: Kernel polling (e.g., Process Monitor).

### 3.2 Moving Average Tracking (Resilience)
The AI tracks a **moving average** of execution time for every thread count. This makes the scheduler resilient to outliers and system noise, as it requires repeated evidence to shift the "optimal" configuration.

### 3.3 Overhead Awareness & Scoring
The AI calculates a **Heuristic Score** to penalize over-parallelization:
`Score = Average_Execution_Time + (Thread_Count * OVERHEAD_CONSTANT)`
This ensures that the scheduler only increases thread counts if the parallel speedup significantly outweighs the cost of management and context switching.

### 3.4 Shared Intelligence in Action
Because the AI table is keyed by `(WorkloadClass, Bucket)`, learning is shared. 
> **Example**: If the AI spends 5 rounds optimizing a 600MB "File Copy", and the user then starts a 600MB "Backup", the system will instantly use the optimal thread count learned from the copy operation, as both belong to `CLASS_IO_BOUND`.

## 4. Technical Stack
- **Language**: C
- **Threading**: POSIX Threads (`pthreads`)
- **System Requirements**: Standard Linux/Unix environment or MinGW for Windows.
