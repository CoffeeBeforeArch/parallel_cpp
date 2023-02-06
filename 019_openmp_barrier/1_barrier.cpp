// Parallel gaussian elimination
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
        // Only master thread calculates a pivot
        #pragma omp master
        {
          // Get the value of the pivot
          auto pivot = m[row * dim + row];

          // Divide the rest of the row by the pivot
          for (int col = row; col < dim; col++) {
            m[row * dim + col] /= pivot;
          }
        }

        // Wait for master thread to finish
        #pragma omp barrier

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
        #pragma omp master
        { row += 1; }

        // Wait for the master thread to update the row
        #pragma omp barrier
      }
    }
  }
}
BENCHMARK(baseline)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
