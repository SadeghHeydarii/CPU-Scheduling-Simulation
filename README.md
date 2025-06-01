# 🧠 CPU Scheduling Algorithms Simulation

This project is a realistic multithreaded simulation of CPU scheduling algorithms implemented in C++. It provides detailed and fair comparisons between different scheduling strategies using identical randomized input.

---

## 📌 Implemented Algorithms

- **First Come First Serve (FCFS)**
- **Shortest Job First (SJF)**
- **Round Robin (RR)** with a fixed quantum (2 seconds)

---

## ⚙️ Project Features

- 🧵 **Multithreaded Simulation:** Each algorithm runs in its own thread using `pthread` for better concurrency and realism.
- 🔐 **Thread-Safe Queues with Mutex:** To avoid race conditions, all access to process queues is secured using `pthread_mutex_t`.
- 🎲 **Randomized but Synchronized Input:** A shared pseudo-random generator (`srand(time(NULL)`) ensures identical process data is used across all three algorithms for fair comparison.
- ⏱️ **Real-Time Execution:** Simulation uses `std::chrono::steady_clock` to simulate actual time progression and delays for process execution.
- 📊 **Performance Metrics Calculation:** For each algorithm, the following metrics are computed:
  - Average Waiting Time
  - Average Turnaround Time
  - Average Response Time

---

## 🛠️ Compilation & Run Instructions

### ✅ Compile

```bash
g++ main.cpp -o scheduler -lpthread
```

> Make sure you're using a C++11-compatible compiler or higher.

### ▶️ Run

```bash
./scheduler
```

---

## 🧾 How to Use

After launching the program, you will be asked to input the number of processes:

```
Enter number of processes: 5
```

The same list of processes will then be automatically simulated under each algorithm (FCFS, SJF, RR) in sequence. Each algorithm outputs its execution log and the calculated average times.

---

## 🔎 Sample Input/Output

### 🔢 Input

```
Enter number of processes: 3
```

### 🖥️ Output

```text
[Creating Processes...]
[Created]   Process 1 at time 0 (burst = 2)
[Created]   Process 2 at time 2 (burst = 2)
[Created]   Process 3 at time 3 (burst = 1)

[FCFS]
[Running]   Process 1 starts at time 0 (burst = 2)
[Complete]  Process 1 finished at time 2
[Running]   Process 2 starts at time 2 (burst = 2)
[Complete]  Process 2 finished at time 4
[Running]   Process 3 starts at time 4 (burst = 1)
[Complete]  Process 3 finished at time 5

[Average Times]
Average Waiting Time   : 1.33333
Average Turnaround Time: 3.33333
Average Response Time  : 1.33333

[SJF]
[Running]   Process 1 starts at time 0 (burst = 2)
[Complete]  Process 1 finished at time 2
[Running]   Process 3 starts at time 3 (burst = 1)
[Complete]  Process 3 finished at time 4
[Running]   Process 2 starts at time 4 (burst = 2)
[Complete]  Process 2 finished at time 6

[Average Times]
Average Waiting Time   : 1.33333
Average Turnaround Time: 3.33333
Average Response Time  : 1.00000

[Round Robin]
[Running]   Process 1 starts at time 0 (burst = 2)
[Complete]  Process 1 finished at time 2
[Running]   Process 2 starts at time 2 (burst = 2)
[Complete]  Process 2 finished at time 4
[Running]   Process 3 starts at time 4 (burst = 1)
[Complete]  Process 3 finished at time 5

[Average Times]
Average Waiting Time   : 1.33333
Average Turnaround Time: 3.33333
Average Response Time  : 1.00000
```

---

## 📐 Time Metrics Explanation

| Metric              | Definition                                                                 |
|---------------------|----------------------------------------------------------------------------|
| **Waiting Time**     | Total time a process spends in the ready queue before execution starts.   |
| **Turnaround Time**  | Total time from arrival to completion.                                    |
| **Response Time**    | Time from arrival to the first time the process starts executing.         |

---

## 🧰 Code Structure (main.cpp)

All algorithms are implemented inside a single `main.cpp` file and executed sequentially. You can easily navigate to specific functions like:

- `runFCFS()`
- `runSJF()`
- `runRR()`

---

## 🎛️ Configuration

- **To change the RR quantum**, edit the macro in the source file:

```cpp
#define QUANTUM 2
```

- **To manually define processes instead of random**, you can replace the logic in `generateProcesses()`.

---

## 👨‍💻 Developer Notes

- Developed for an Operating Systems course assignment to simulate scheduling strategies.
- Demonstrates real-world issues like synchronization, timing, thread safety, and performance tradeoffs.
- Designed to be modular and extendable for additional scheduling strategies (e.g., Priority Scheduling).
