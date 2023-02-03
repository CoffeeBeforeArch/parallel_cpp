// A parallel program with a data race
// By: Nick from CoffeeBeforeArch

#include <cassert>
#include <thread>
#include <vector>

int main() {
  // Number of total increments and increments per thread
  const int num_increments = 1 << 20;
  const int num_threads = 8;
  const int increments_per_thread = num_increments / num_threads;

  // Incremented by each thread
  volatile int sink = 0;

  // Create a work function to add elements
  auto work = [&]() {
    for (int i = 0; i < increments_per_thread; i++) {
        sink = sink + 1;
    }
  };

  // Spawn threads
  std::vector<std::jthread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work);
  }

  // Join the threads and check the result
  for(auto &thread : threads) thread.join();
  assert(sink == num_increments);
  return 0;
}
