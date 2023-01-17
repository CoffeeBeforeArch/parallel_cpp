// A false sharing benchmark
// By: Nick from CoffeeBeforeArch

#include <array>
#include <atomic>
#include <thread>
#include <vector>

int main() {
    // Number of total iterations to run
    const int num_iterations = 1 << 27;
    
    // Number of threads to spawn
    const int num_threads = 4;

    // Atomic integers to increment
    std::array<std::atomic<int>, 4> counters = {0, 0, 0, 0};
    std::atomic<int> final_sum = 0;

    // Number of elements to process per thread
    const int elements_per_thread = num_iterations / num_threads;

    // Lambda for our work
    auto work = [&](int thread_id) {
        for (int i = 0; i < elements_per_thread; i++) {
            counters[thread_id]++;
        }
        final_sum += counters[thread_id];
    };

    // Spawn threads
    std::vector<std::jthread> threads;
    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(work, i);
    }

    return 0;
}
