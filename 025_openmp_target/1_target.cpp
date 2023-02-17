// Target offloading example
// By: Nick from CoffeeBeforeArch

#include <omp.h>

#include <algorithm>
#include <iostream>
#include <random>

int main() {
  // Allocate our arrays
  const int N = 1 << 26;
  float *a = new float[N];
  float *b = new float[N];
  float *c = new float[N];

  // Create our random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution dist(1.0f, 2.0f);

  // Initialize a and b
  std::generate(a, a + N, [&] { return dist(mt); });
  std::generate(b, b + N, [&] { return dist(mt); });

  // Get time before
  auto start = omp_get_wtime();

  // Do vector addition
  #pragma omp target teams \
  distribute parallel for simd \
  map(to: a [0:N], b [0:N]) map(from: c [0:N])
  for (int i = 0; i < N; i++) {
    c[i] = a[i] + b[i];
  }

  // Get time after
  auto end = omp_get_wtime();
  std::cout << end - start << '\n';

  // Free our memory
  delete[] a;
  delete[] b;
  delete[] c;
  return 0;
}
