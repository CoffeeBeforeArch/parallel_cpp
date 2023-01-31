// Dot product implementation with auto-vectorization
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <algorithm>
#include <execution>
#include <random>
#include <vector>

static void dot_product(benchmark::State &s) {
  // Create random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution dist(0.0f, 1.0f);

  // Create vectors of random numbers
  const int num_elements = 1 << 15;
  std::vector<float> v1;
  std::vector<float> v2;
  std::ranges::generate_n(std::back_inserter(v1), num_elements,
                          [&] { return dist(mt); });
  std::ranges::generate_n(std::back_inserter(v2), num_elements,
                          [&] { return dist(mt); });

  // Perform dot product
  float *sink = new float;
  for (auto _ : s) {
    *sink = std::transform_reduce(std::execution::unseq, v1.begin(), v1.end(),
                                  v2.begin(), 0.0f);
  }
}
BENCHMARK(dot_product)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
