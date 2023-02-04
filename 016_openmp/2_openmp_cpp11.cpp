// An example using OpenMP
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
  std::generate_n(std::back_inserter(v_in), num_elements,
                  [&] { return dist(mt); });

  // Output vector is just 0s
  std::vector<float> v_out(num_elements);

  // Timing loop
  for (auto _ : s) {
    // Parallelize the for loop
    [[ omp::sequence(directive(parallel), directive(for)) ]]
    for (int i = 0; i < num_elements; i++) {
      // Square v_in and set v_out
      v_out[i] = v_in[i] * v_in[i];
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMicrosecond)->UseRealTime();

BENCHMARK_MAIN();
