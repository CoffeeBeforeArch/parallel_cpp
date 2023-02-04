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
  std::vector<float> v_in;
  std::generate_n(std::back_inserter(v_in), num_elements, [&]{return dist(mt);});

  // Timing loop
  for (auto _ : s) {
    // Create our variable to accumulate into
    float sink = 0;
    
    // Run the sum of squares
    for (int i = 0; i < num_elements; i++) {
      // Square v_in and add to sink
      benchmark::DoNotOptimize(sink += v_in[i] * v_in[i]);
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMicrosecond)->UseRealTime();

BENCHMARK_MAIN();
