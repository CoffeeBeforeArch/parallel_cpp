// Vectorized implementation of a dot product
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <algorithm>
#include <execution>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

static void dp_bench(benchmark::State &s) {
  // Create a random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution dist(1, 10);

  // Create vectors of random numbers
  const int num_elements = 1 << 15;
  std::vector<int> v1;
  std::vector<int> v2;
  std::ranges::generate_n(std::back_inserter(v1), num_elements,
                          [&] { return dist(mt); });
  std::ranges::generate_n(std::back_inserter(v2), num_elements,
                          [&] { return dist(mt); });

  // Perform dot product
  int *sink = new int;
  for (auto _ : s) {
    *sink = std::transform_reduce(std::execution::unseq, v1.begin(), v1.end(),
                                  v2.begin(), 0);
  }
}
BENCHMARK(dp_bench)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
