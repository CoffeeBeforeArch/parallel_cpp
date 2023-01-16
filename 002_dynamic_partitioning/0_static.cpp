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

  // Number of elements to process
  int num_work_items = 1 << 18;
  
  // Number of threads to spawn
  int num_threads = 8;

  // Create work items
  std::vector<int> work_items;
  work_items.reserve(num_work_items);
  for(int i = 0; i < num_work_items; i+= num_threads) {
      // Threads 0/1 get all the data from bin 1
      work_items.push_back(bin_1(mt));
      work_items.push_back(bin_1(mt));

      // Threads 2/3 get all the data from bin 2
      work_items.push_back(bin_2(mt));
      work_items.push_back(bin_2(mt));

      // Threads 4/5 get all the data from bin 3
      work_items.push_back(bin_3(mt));
      work_items.push_back(bin_3(mt));
      
      // Threads 6/7 get all the data from bin 4
      work_items.push_back(bin_4(mt));
      work_items.push_back(bin_4(mt));
  }

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
