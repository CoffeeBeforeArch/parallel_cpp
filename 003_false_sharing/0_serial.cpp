// A serial execution benchmark
// By: Nick from CoffeeBeforeArch

#include <atomic>
#include <thread>
#include <vector>

int main() {
    // Number of total iterations to run
    const int num_iterations = 1 << 27;
    
    // Number of threads to spawn
    const int num_threads = 1;

    // Atomic integer to increment
    std::atomic<int> counter = 0;

    // Lambda for our work
    auto work = [&]() {
        for (int i = 0; i < num_iterations; i++) {
            counter++;
        }
    };

    // Spawn threads
    std::vector<std::jthread> threads;
    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(work);
    }

    return 0;
}
