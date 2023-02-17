// Baseline vector addition
// By: Nick from CoffeeBeforeArch

#include <omp.h>

#include <algorithm>
#include <iostream>
#include <random>

int main() {
  // Create some large arrays
  const int num_elements = 1 << 26;
  float *a = new float[num_elements];
  float *b = new float[num_elements];
  float *c = new float[num_elements];

  // Create our random numbers
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution dist(1.0f, 2.0f);

  // Initialize a and b
  std::generate(a, a + num_elements, [&] { return dist(mt); });
  std::generate(b, b + num_elements, [&] { return dist(mt); });

  // Get time before
  auto start = omp_get_wtime();

  // Do vector addition
  for (int i = 0; i < num_elements; i++) {
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
