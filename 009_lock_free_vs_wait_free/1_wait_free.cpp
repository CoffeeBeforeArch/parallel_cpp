// An example with non-blocking
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
    for(int i = 0; i < items_per_thread; i++) sink++;
  };

  // Create threads
  std::vector<std::jthread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work);
  }
  return 0;
}
