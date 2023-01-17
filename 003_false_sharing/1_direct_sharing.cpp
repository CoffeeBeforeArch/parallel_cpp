// A direct sharing benchmark
// By: Nick from CoffeeBeforeArch

#include <atomic>
#include <thread>
#include <vector>

int main() {
    // Number of total iterations to run
    const int num_iterations = 1 << 27;
    
    // Number of threads to spawn
    const int num_threads = 4;

    // Number of elements to process per thread
    const int elements_per_thread = num_iterations / num_threads;

    // Atomic integer to increment
    std::atomic<int> counter = 0;

    // Lambda for our work
    auto work = [&]() {
        for (int i = 0; i < elements_per_thread; i++) {
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
