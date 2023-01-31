// Dot product implementation with SIMD intrinsics
// By: Nick from CoffeeBeforeArch

#include <immintrin.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

// Hand-tuned dot product
float dot_product(const __m256 *v1, const __m256 *v2,
                  size_t num_packed_elements) {
  auto tmp = 0.0f;
  for (size_t i = 0; i < num_packed_elements; i++) {
    // Temporary variables to help with intrinsic
    float r[8];
    __m256 rv;

    // Our dot product intrinsic
    rv = _mm256_dp_ps(v1[i], v2[i], 0xf1);

    // Extract __m256 into array of floats
    std::memcpy(r, &rv, sizeof(float) * 8);

    // Perform the final add
    tmp += r[0] + r[4];
  }
  return tmp;
}

int main() {
  // Get the size of the vector
  const size_t num_elements = 1 << 20;
  const size_t elements_per_reg = 8;
  const size_t num_packed_elements = num_elements / elements_per_reg;

  // Create random number generator
  std::mt19937 mt(123);
  std::uniform_real_distribution dist(0.0f, 1.0f);

  // Allocate __m256 aligned to 32B
  auto v1 = static_cast<__m256 *>(
      std::aligned_alloc(32, num_packed_elements * sizeof(__m256)));
  auto v2 = static_cast<__m256 *>(
      std::aligned_alloc(32, num_packed_elements * sizeof(__m256)));

  // Create random numbers for v1 and v2
  // Use 2 loops to preserve ordering of randomly generated numbers
  for (size_t i = 0; i < num_packed_elements; i++) {
    v1[i] = _mm256_set_ps(dist(mt), dist(mt), dist(mt), dist(mt), dist(mt),
                          dist(mt), dist(mt), dist(mt));
  }
  for (size_t i = 0; i < num_packed_elements; i++) {
    v2[i] = _mm256_set_ps(dist(mt), dist(mt), dist(mt), dist(mt), dist(mt),
                          dist(mt), dist(mt), dist(mt));
  }


  // Perform dot product
  float result = dot_product(v1, v2, num_packed_elements);
  std::cout << result << '\n';

  // Free memory when we're done
  free(v1);
  free(v2);
}
