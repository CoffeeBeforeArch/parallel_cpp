// Dot product implementation with SIMD intrinsics
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>
#include <immintrin.h>

#include <cstdlib>
#include <cstring>
#include <random>

// Hand-tuned dot product
float dot_product(const __m256 *v1, const __m256 *v2,
                  size_t num_packed_elements) {
  auto tmp = 0.0f;
  for (size_t i = 0; i < num_packed_elements; i++) {
    // Array to unpack __m256 result into
    float unpacked[8];
    __m256 result;

    // Our dot product intrinsic
    result = _mm256_dp_ps(v1[i], v2[i], 0xf1);

    // Extract __m256 into array of floats
    std::memcpy(unpacked, &result, sizeof(float) * 8);

    // Perform the final add
    tmp += unpacked[0] + unpacked[4];
  }
  return tmp;
}

static void dot_product_bench(benchmark::State &s) {
  // Get the size of the vector
  const size_t num_elements = 1 << 15;
  const size_t elements_per_reg = 8;
  const size_t num_packed_elements = num_elements / elements_per_reg;

  // Create random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution dist(0.0f, 1.0f);

  // Allocate __m256 aligned to 32B
  auto v1 = static_cast<__m256 *>(
      std::aligned_alloc(32, num_packed_elements * sizeof(__m256)));
  auto v2 = static_cast<__m256 *>(
      std::aligned_alloc(32, num_packed_elements * sizeof(__m256)));

  // Create random numbers for v1 and v2
  for (size_t i = 0; i < num_packed_elements; i++) {
    v1[i] = _mm256_set_ps(dist(mt), dist(mt), dist(mt), dist(mt), dist(mt),
                          dist(mt), dist(mt), dist(mt));
    v2[i] = _mm256_set_ps(dist(mt), dist(mt), dist(mt), dist(mt), dist(mt),
                          dist(mt), dist(mt), dist(mt));
  }

  // Perform dot product
  float *result = new float;
  for (auto _ : s) {
    *result = dot_product(v1, v2, num_packed_elements);
  }

  // Free memory when we're done
  free(v1);
  free(v2);
}
BENCHMARK(dot_product_bench)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
