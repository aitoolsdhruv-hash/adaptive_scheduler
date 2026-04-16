# Adaptive Task Scheduler (ATS)

A sophisticated, Behavioral-Oriented C Scheduler with Heuristic AI that generalizes hardware learning across OS workload classes.

## 🚀 Quick Start (Production Binaries)

I have pre-compiled the hardened binaries for you in the `bin/` folder.

### 1. Interactive Dashboard (Presentation Mode)
Run the full adaptive menu to see the learning in action.
```powershell
.\bin\scheduler_dashboard.exe
```

### 2. High-Concurrency Stress Test
Instantly flood the system with 50 concurrent jobs to verify stability and backpressure.
```powershell
.\bin\scheduler_stress.exe
```

### 3. Logic Verification
Run the standalone unit test suite to verify the AI math and decomposer ranges.
```powershell
.\bin\unit_tests.exe
```

## 🛠️ Build System

If you modify the source code, use the following `make` commands (requires MinGW/w64devkit):

```powershell
make all        # Build both production binaries
make dashboard  # Build only the interactive menu
make stress     # Build only the auto-stress test
make verify     # Build and run the logic verification suite
make clean      # Clear all binaries and object files
```

## 🧠 Core Architecture

ATS is built on a modular **Producer-Consumer-Dispatcher** architecture with a specialized **Shared Intelligence** layer.

| Module | Responsibility |
| :--- | :--- |
| **`ai_model.c`** | Learned heuristic engine; Calculates optimal parallelism with overhead penalty. |
| **`utils.c`** | Global system state, thread-safe synchronization, and timestamping. |
| **`decomposer.c`** | Mathematical slicing of workloads into 0-indexed, non-overlapping thread ranges. |
| **`queue.c`** | Hardened circular buffer with blocking "Full Queue" backpressure. |
| **`thread_pool.c`** | Management of 8 persistent workers and subtask lifecycle management. |

## 📊 Documentation

- **[Deep Dive](PROJECT_DETAILS.md)**: Architectural detail and AI scoring formulas.
- **[Design Rationale](docs/rationale.md)**: Why we chose Moving Averages and specific Overhead Constants.
- **[Latest Walkthrough](walkthrough.md)**: Detailed report on the most recent logic verification and hardening steps.

---
**Build for OS Course Project - Version 2.0 (Hardened)**
