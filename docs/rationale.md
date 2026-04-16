# Design Rationale: Adaptive Task Scheduler

This document explains the technical and academic reasoning behind the specific architectural choices made during the development of this project.

## 1. Why use a Moving Average for Performance Tracking?
In a primitive model, a single "lucky" run with 8 threads (where no other background tasks were competing for CPU) could permanently skew the AI's understanding of that task.

By using a **Moving Average**, we ensure:
- **Resilience to Outliers**: One slow run doesn't ruin the profile's confidence.
- **Continuous Adaptation**: As the system ages or background load shifts, the average gradually trends toward reality.
- **Mathematical Formula**:
  `avg[n] = ((avg[n] * count[n]) + latest_time) / (count[n] + 1)`

## 2. The Physics of the `OVERHEAD_CONSTANT` (5.0ms)
Parallelization is not free. In an Operating System, launching a thread involves:
- **Context Switching**: Saving/restoring registers and memory states.
- **Synchronization**: Contention over mutexes and condition variables.

In a real OS, a context switch takes roughly 10-50 microseconds. However, in our **scaled simulation**, we use **5.0ms** as the overhead per thread. This serves a critical pedagogical purpose: it forces the AI to "prove" that adding a thread will save at least 5ms of raw execution time. If 4 threads take 48ms and 5 threads take 47ms, the AI will reject the 5th thread because the overhead cost exceeds the performance gain.

## 3. Workload Classes vs. Process Types
Standard schedulers often try to optimize based on the process name (e.g., `copy.exe`). This is brittle.

Our system abstracts these into **Behavioral Classes**:
- `IO_BOUND`
- `CPU_BOUND`
- `DIR_TRAVERSAL`

This allows for **Shared Global Intelligence**. If the AI learns how to optimize a large data copy, it can apply those same lessons to a "Media Indexing" task because they both share high sequential I/O patterns.

## 4. Blocking Queue on Overflow
Most academic projects simply drop new jobs if a queue is full. This is unrealistic for a real-time OS. By using **Producer Blocking** (via `queue_full_cond`), we ensure that:
- No data is lost.
- The system exerts "Backpressure" on the job generator.
- Threads are fully utilized before the producer is allowed to continue, maximizing throughput.
