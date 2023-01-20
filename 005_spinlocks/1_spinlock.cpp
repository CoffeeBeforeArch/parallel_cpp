// A simple example using a spinlock
// By:Nick from CoffeeBeforeArch

#include <pthread.h>

#include <algorithm>
#include <chrono>
#include <list>
#include <mutex>
#include <random>
#include <thread>

int main() {
  // Random number generate
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution dist(10, 20);

  // Vector of random numbers
  std::list<int> l;
  std::generate_n(std::back_inserter(l), 1 << 20, [&]() { return dist(mt); });

  // Our lock guarding access to the list
  pthread_spinlock_t spinlock;
  pthread_spin_init(&spinlock, 0);

  // Function that removes items from the list in parallel
  auto work = [&]() {
    while (true) {
      // Grab the lock before doing anything
      pthread_spin_lock(&spinlock);

      // Check if there are no more items
      if (l.empty()) {
        pthread_spin_unlock(&spinlock);
        break;
      }

      // Remove the item at the end of the list
      l.pop_back();
      pthread_spin_unlock(&spinlock);
    }
  };

  // Spawn threads
  int num_threads = 8;
  std::vector<std::jthread> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(work);
  }
  return 0;
}
