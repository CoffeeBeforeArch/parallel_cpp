// A simple example using a concurrent queue
// By: Nick from CoffeeBeforeArch

#include <tbb/concurrent_queue.h>

#include <random>
#include <thread>
#include <vector>

int main() {
  // Number of elements to add to the queue
  const int num_elements = 1 << 25;

  // Divide the work by threads
  const int num_threads = 8;
  const int elements_per_thread = num_elements / num_threads;

  // Create a concurrent queue
  tbb::concurrent_queue<int> queue;

  // Create a random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution dist(1, 100);

  // Create a work function to add elements
  auto work = [&]() {
    for (int i = 0; i < elements_per_thread; i++) {
      queue.push(dist(mt));
    }
  };

  // Spawn threads
  std::vector<std::jthread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work);
  }
  return 0;
}
