// A simple using using a spinlock under heavy contention
// By: Nick from CoffeeBeforeArch

#include <pthread.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  // Number of iterations
  const int num_iterations = 1 << 25;

  // Number of threads and iterations per thread
  const int num_threads = 8;
  const int iterations_per_thread = num_iterations / num_threads;

  // Keep track of per-thread average wait time
  std::array<int, 8> max_wait_time;

  // Our lock guarding access to the list
  pthread_spinlock_t spinlock;
  pthread_spin_init(&spinlock, 0);

  // Function that removes items from the list in parallel
  auto work = [&](int tid) {
    int max = 0;
    for (int i = 0; i < iterations_per_thread; i++) {
      // Time how long it takes to get the lock
      auto start = std::chrono::system_clock::now();
      pthread_spin_lock(&spinlock);
      auto stop = std::chrono::system_clock::now();

      // Unlock the thread
      pthread_spin_unlock(&spinlock);

      // Calculate how long it took to get the lock
      int diff =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
              .count();

      // Add to the running total and keep track of the max
      max = std::max(max, diff);
    }

    // Save the max wait time and the average wait time
    max_wait_time[tid] = max;
  };

  // Spawn threads
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work, i);
  }

  // Join threads
  for (auto &thread : threads) {
    thread.join();
  }

  // Print out the max wait times
  for (auto max : max_wait_time) {
    std::cout << max << ' ';
  }
  std::cout << '\n';
  return 0;
}
