#include <iostream>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"
#define QUANTUM 2

struct process {
    int id;
    int burst_time;
    time_point<steady_clock> arrival_time;
    time_point<steady_clock> last_execution;
    int round;

    bool operator<(const process& other) const {
        return burst_time > other.burst_time;
    }
};

queue<process> q;
priority_queue<process> pq;

int done = 0;
int waiting_time = 0;
int response_time = 0;
int turnaround_time = 0;

pthread_mutex_t queue_lock;
pthread_mutex_t pqueue_lock;

steady_clock::time_point global_start_time;

int get_elapsed_seconds(time_point<steady_clock> t) {
    return duration_cast<seconds>(t - global_start_time).count();
}

void print_execution(const string& status, const process& p, int time, const string& algo = "", bool Quantum = false) {
    if (!algo.empty())
        cout << RED << "[" << algo << "] " << RESET;

    if (status == "start") {
        if (Quantum)
            cout << CYAN << "[Running]   Process " << p.id << " starts at time " << time
                 << " (burst = " << QUANTUM << ")" << RESET << endl;
        else
            cout << CYAN << "[Running]   Process " << p.id << " starts at time " << time
                 << " (burst = " << p.burst_time << ")" << RESET << endl;
    } else if (status == "complete")
        cout << GREEN << "[Complete]  Process " << p.id << " finished at time " << time << RESET << endl;
    else if (status == "return")
        cout << GREEN << "[Return]    Process " << p.id << " returned to queue at time " << time << RESET << endl;
    else if (status == "create")
        cout << MAGENTA << "[Created]   Process " << p.id << " at time " << time
             << " (burst = " << p.burst_time << ")" << RESET << endl;
}

void* FCFS(void* arg) {
    int n = *(int*)arg;
    while (done != n) {
        pthread_mutex_lock(&queue_lock);
        if (!q.empty()) {
            process prc = q.front(); q.pop();
            pthread_mutex_unlock(&queue_lock);

            auto start_time = steady_clock::now();
            int wait = get_elapsed_seconds(start_time) - get_elapsed_seconds(prc.arrival_time);

            response_time += wait;
            waiting_time += wait;
            sleep(prc.burst_time);

            auto end_time = steady_clock::now();
            int completion = get_elapsed_seconds(end_time);
            int turnaround = completion - get_elapsed_seconds(prc.arrival_time);

            turnaround_time += turnaround;
            done++;

            print_execution("start", prc, get_elapsed_seconds(start_time), "FCFS");
            print_execution("complete", prc, completion, "FCFS");
        } else {
            pthread_mutex_unlock(&queue_lock);
            usleep(500);
        }
    }
    return nullptr;
}

void* SJF(void* arg) {
    int n = *(int*)arg;
    while (done != n) {
        pthread_mutex_lock(&pqueue_lock);
        if (!pq.empty()) {
            process prc = pq.top(); pq.pop();
            pthread_mutex_unlock(&pqueue_lock);

            auto start_time = steady_clock::now();
            int wait = get_elapsed_seconds(start_time) - get_elapsed_seconds(prc.arrival_time);

            response_time += wait;
            waiting_time += wait;
            sleep(prc.burst_time);

            auto end_time = steady_clock::now();
            int completion = get_elapsed_seconds(end_time);
            int turnaround = completion - get_elapsed_seconds(prc.arrival_time);

            turnaround_time += turnaround;
            done++;

            print_execution("start", prc, get_elapsed_seconds(start_time), "SJF");
            print_execution("complete", prc, completion, "SJF");
        } else {
            pthread_mutex_unlock(&pqueue_lock);
            usleep(500);
        }
    }
    return nullptr;
}

void* RR(void* arg) {
    int n = *(int*)arg;
    while (done != n) {
        pthread_mutex_lock(&queue_lock);
        if (!q.empty()) {
            process prc = q.front(); q.pop();
            pthread_mutex_unlock(&queue_lock);

            auto start_time = steady_clock::now();
            int now = get_elapsed_seconds(start_time);
            int last = get_elapsed_seconds(prc.round == 0 ? prc.arrival_time : prc.last_execution);

            if (prc.round == 0)
                response_time += (now - last);

            waiting_time += (now - last);

            if (prc.burst_time <= QUANTUM) {
                sleep(prc.burst_time);
                int completion = get_elapsed_seconds(steady_clock::now());
                turnaround_time += (completion - get_elapsed_seconds(prc.arrival_time));
                done++;

                print_execution("start", prc, now, "RR");
                print_execution("complete", prc, completion, "RR");
            } else {
                sleep(QUANTUM);
                prc.burst_time -= QUANTUM;
                prc.round++;
                prc.last_execution = steady_clock::now();

                pthread_mutex_lock(&queue_lock);
                q.push(prc);
                pthread_mutex_unlock(&queue_lock);

                print_execution("start", prc, now, "RR", true);
                print_execution("return", prc, get_elapsed_seconds(prc.last_execution), "RR");
            }
        } else {
            pthread_mutex_unlock(&queue_lock);
            usleep(500);
        }
    }
    return nullptr;
}

void print_averages(int n) {
    cout << YELLOW << "\n[Average Times]\n" << RESET;
    cout << "Average Waiting Time   : " << (float)waiting_time / n << endl;
    cout << "Average Turnaround Time: " << (float)turnaround_time / n << endl;
    cout << "Average Response Time  : " << (float)response_time / n << endl;
}

void create_processes(int n, bool forSJF) {
    cout << YELLOW << "\n[Creating Processes...]\n" << RESET;
    for (int i = 0; i < n; i++) {
        auto now = steady_clock::now();
        int burst = rand() % 3 + 1;
        process p = {i + 1, burst, now, now, 0};

        print_execution("create", p, get_elapsed_seconds(now));

        if (forSJF) {
            pthread_mutex_lock(&pqueue_lock);
            pq.push(p);
            pthread_mutex_unlock(&pqueue_lock);
        } else {
            pthread_mutex_lock(&queue_lock);
            q.push(p);
            pthread_mutex_unlock(&queue_lock);
        }
        sleep(rand() % 3);
    }
}

void reset() {
    done = 0;
    waiting_time = 0;
    turnaround_time = 0;
    response_time = 0;
    while (!q.empty()) q.pop();
    while (!pq.empty()) pq.pop();
    global_start_time = steady_clock::now();
}

int main() {
    const int NUM_THREADS = 3;
    pthread_t threads[NUM_THREADS];
    int seed = time(NULL);

    int n;
    cout << MAGENTA << "Enter number of processes: " << RESET;
    cin >> n;

    pthread_mutex_init(&queue_lock, nullptr);
    pthread_mutex_init(&pqueue_lock, nullptr);

    srand(seed);
    reset();
    pthread_create(&threads[0], NULL, FCFS, &n);
    create_processes(n, false);
    pthread_join(threads[0], NULL);
    print_averages(n);

    srand(seed);
    reset();
    pthread_create(&threads[1], NULL, SJF, &n);
    create_processes(n, true);
    pthread_join(threads[1], NULL);
    print_averages(n);

    srand(seed);
    reset();
    pthread_create(&threads[2], NULL, RR, &n);
    create_processes(n, false);
    pthread_join(threads[2], NULL);
    print_averages(n);

    pthread_mutex_destroy(&queue_lock);
    pthread_mutex_destroy(&pqueue_lock);

}
