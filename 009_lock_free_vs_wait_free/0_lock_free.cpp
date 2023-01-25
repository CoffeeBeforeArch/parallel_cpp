// An example with of lock-free
// By: Nick from CoffeeBeforeArch

#include <atomic>
#include <cassert>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

int main() {
  // Set up the number of increments
  const int iterations = 1 << 25;
  const int num_threads = 8;
  const int items_per_thread = iterations / num_threads;

  // Shared value to increment
  std::atomic<int> sink = 0;

  // Normal work function
  auto work = [&]() {
    for(int i = 0; i < items_per_thread; i++) {
      // Start by reading the current value
      int desired;
      int expected = sink.load();
      do {
        // Update the current desired value
        desired = expected + 1;
        // Try CAS until successful
      } while (!sink.compare_exchange_strong(expected, desired));
    }
  };

  // Create threads
  std::vector<std::jthread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work);
  }
  return 0;
}
