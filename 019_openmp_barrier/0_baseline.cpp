// Serial gaussian elimination
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>
#include <vector>

static void baseline(benchmark::State &s) {
  // Create a random number generator
  std::mt19937 mt(123);
  std::uniform_real_distribution dist(1.0f, 2.0f);

  // Create a matrix
  std::vector<float> m_in;
  const int dim = 1 << 10;
  std::generate_n(std::back_inserter(m_in), dim * dim,
                  [&] { return dist(mt); });

  // Timing loop
  for (auto _ : s) {
    // Copy in the matrix each iteration
    std::vector<float> m = m_in;

    // Performance gaussian elimination
    for (int row = 0; row < dim; row++) {
      // Get the value of the pivot
      auto pivot = m[row * dim + row];

      // Divide the rest of the row by the pivot
      for (int col = row; col < dim; col++) {
        m[row * dim + col] /= pivot;
      }

      // Eliminate the pivot col from the remaining rows
      for (int elim_row = row + 1; elim_row < dim; elim_row++) {
        // Get the scaling factor for elimination
        auto scale = m[elim_row * dim + row];

        // Remove the pivot
        for (int col = row; col < dim; col++) {
          m[elim_row * dim + col] -= m[row * dim + col] * scale;
        }
      }
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
