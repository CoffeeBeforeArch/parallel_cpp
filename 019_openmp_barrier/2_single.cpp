// Parallel gaussian elimination with single
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
    // Copy in the vector each iteration
    std::vector<float> m = m_in;
    
    // Set the starting row to 0 (shared between threads)
    int row = 0;
    #pragma omp parallel
    {
      // While there are still rows to process...
      while (row < dim) {
        // A single thread does the pivot calculations
        #pragma omp single
        {
          // Get the value of the pivot
          auto pivot = m[row * dim + row];

          // Divide the rest of the row by the pivot
          for (int col = row; col < dim; col++) {
            m[row * dim + col] /= pivot;
          }
        }

        // Eliminate the pivot col from the remaining rows
        #pragma omp for
        for (int elim_row = row + 1; elim_row < dim; elim_row++) {
          // Get the scaling factor for elimination
          auto scale = m[elim_row * dim + row];

          // Remove the pivot
          for (int col = row; col < dim; col++) {
            m[elim_row * dim + col] -= m[row * dim + col] * scale;
          }
        }

        // Update the row each iteration
        #pragma omp single
        { row += 1; }
      }
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
