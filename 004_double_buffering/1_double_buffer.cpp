// A simple example of double buffering in C++
// By: Nick from CoffeeBeforeArch

#include <random>
#include <semaphore>
#include <span>
#include <thread>
#include <vector>

// Function for generating data
void generate_data(std::span<int> data) {
  // Create random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution dist(1, 100);

  // Generate random data
  for (auto &value : data) {
    value = dist(mt);
  }
}

// Function for processing data (dummy work)
void process_data(std::span<int> data) {
  for (int i = 0; i < 5; i++) {
    for (auto &value : data) {
      value %= value + 1;
    }
  }
}

int main() {
  // Create std::vector passed between generation/processing
  std::vector<int> data_1;
  std::vector<int> data_2;
  data_1.resize(1 << 20);
  data_2.resize(1 << 20);

  // Semaphores for coordinating work
  std::binary_semaphore signal_to_process{0};
  std::binary_semaphore signal_to_generate{1};

  // Number of iterations to perform
  const int num_iterations = 100;

  // Work function for generating data
  auto data_generator = [&]() {
    // Run for 100 iterations
    for (int i = 0; i < num_iterations; i++) {
      // Generate data
      generate_data(data_1);

      // Wait until current processing work is done
      signal_to_generate.acquire();

      // Swap the vectors
      data_1.swap(data_2);

      // Signal the processing thread to begin
      signal_to_process.release();
    }
  };

  // Work function for generating data
  auto data_processor = [&]() {
    // Run for 100 iterations
    for (int i = 0; i < num_iterations; i++) {
      // Wait until the current data generation is done
      signal_to_process.acquire();

      // Process the data
      process_data(data_2);

      // Signal the processing thread to begin
      signal_to_generate.release();
    }
  };

  // Spawn our two threads
  std::jthread generator(data_generator);
  std::jthread processor(data_processor);

  return 0;
}
