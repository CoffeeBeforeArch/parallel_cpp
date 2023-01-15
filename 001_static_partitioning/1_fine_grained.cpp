// Static work distribution with fine-grained round-robin assignment
// By: Nick from CoffeeBeforeArch

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>
#include <span>
#include <thread>
#include <vector>

int main() {
  // Create a random number generator
  std::random_device rd;
  std::mt19937 mt(rd());

  // Create 4 different distributions
  std::uniform_int_distribution bin_1(1, 25);
  std::uniform_int_distribution bin_2(26, 50);
  std::uniform_int_distribution bin_3(51, 75);
  std::uniform_int_distribution bin_4(76, 100);

  // Calculate the number elements per bin
  int num_work_items = 1 << 18;
  int n_bins = 4;
  int elements_per_bin = num_work_items / n_bins;

  // Create work items
  std::vector<int> work_items;
  std::generate_n(std::back_inserter(work_items), elements_per_bin,
                  [&] { return bin_1(mt); });
  std::generate_n(std::back_inserter(work_items), elements_per_bin,
                  [&] { return bin_2(mt); });
  std::generate_n(std::back_inserter(work_items), elements_per_bin,
                  [&] { return bin_3(mt); });
  std::generate_n(std::back_inserter(work_items), elements_per_bin,
                  [&] { return bin_4(mt); });


  // Number of threads to spawn
  int num_threads = 8;
  
  // Create a lambda to process the work items
  auto work = [&](int thread_id) {
      for(int i = thread_id; i < num_work_items; i += num_threads) {
          std::this_thread::sleep_for(std::chrono::microseconds(work_items[i]));
      }
  };
  
  // Spawn threads (join in destructor of jthread)
  std::vector<std::jthread> threads;
  for(int i = 0; i < num_threads; i++) {
      threads.emplace_back(work, i);
  }

  return 0;
}
