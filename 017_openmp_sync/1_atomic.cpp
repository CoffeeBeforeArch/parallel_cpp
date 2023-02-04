// An example with an OpenMP atomic
// By: Nick from CoffeeBeforeArch

#include <cassert>

int main() {
    // Set up the number of iterations to run
    const int num_iterations = 1 << 20;
    const int num_threads = 8;
    const int iterations_per_thread = num_iterations / num_threads;

    // Integer to increment
    int sink = 0;

    // Run this loop in multiple threads
    #pragma omp parallel num_threads(8)
    {
        for(int i = 0; i < iterations_per_thread; i++) {
            // Say this is a critical section
            #pragma omp atomic
            sink++;
        }

    }
    assert(sink == num_iterations);
    return 0;
}
