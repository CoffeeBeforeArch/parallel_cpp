// A simple example of OpenMP reduction
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <random>
#include <vector>

static void baseline(benchmark::State &s) {
  // Create a random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution dist(0.0f, 1.0f);

  // Create vectors of random numbers
  const int num_elements = 1 << 20;
  std::vector<float> v_in(num_elements, 1);

  // Accumulate the results
  float sink = 0;

  // Timing loop
  for (auto _ : s) {
  // Parallelize the for loop
    for (int i = 0; i < num_elements; i++) {
      // Square v_in and set v_out
      sink += v_in[i] * v_in[i];
      benchmark::DoNotOptimize(sink);
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMicrosecond)->UseRealTime();

BENCHMARK_MAIN();
