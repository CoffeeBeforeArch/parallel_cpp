// A baseline implementation in C++
// By: Nick from CoffeeBeforeArch

#include <random>
#include <span>
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
  std::vector<int> data;
  data.resize(1 << 20);

  // Go through 100 iterations of generating/processing data
  for (int i = 0; i < 100; i++) {
    generate_data(data);
    process_data(data);
  }
  return 0;
}
