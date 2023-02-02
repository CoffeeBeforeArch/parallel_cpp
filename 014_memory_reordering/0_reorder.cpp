// A simple example of memory reordering
// By: Nick from CoffeeBeforeArch

#include <cassert>
#include <cstdio>
#include <iostream>
#include <semaphore>
#include <thread>

void reorder(std::binary_semaphore &start, std::counting_semaphore<2> &end,
             int &v1, int &v2, int &rec) {
  // Keep going forever
  while (true) {
    // Wait for the signal to start
    start.acquire();

    // Write to v2
    v1 = 1;

    // Read v1
    rec = v2;

    // Say we're done for this iteration
    end.release();
  }
}

int main() {
  // Semaphores for signaling threads
  std::binary_semaphore start_t0(0);
  std::binary_semaphore start_t1(0);
  std::counting_semaphore<2> done(0);

  // Variable for memory re-ordering
  int v1 = 0;
  int v2 = 0;
  int r1 = 0;
  int r2 = 0;

  // Start threads
  std::thread t1([&] { reorder(start_t0, done, v1, v2, r1); });
  std::thread t2([&] { reorder(start_t1, done, v2, v1, r2); });

  for (int i = 0;; i++) {
    // Zero v1/v2 each iteration
    v1 = 0;
    v2 = 0;

    // Allow threads to continue
    start_t0.release();
    start_t1.release();

    // Wait for both threads to finish
    done.acquire();
    done.acquire();

    // Check for memory reordering
    auto reordering = (r1 == 0) && (r2 == 0);
    if (reordering) {
      std::cout << "ERROR! r1 = " << r1 << ", r2 = " << r2
                << ", Iteration: " << i << '\n';
      assert(false);
    } else
      std::cout << "ALL GOOD! r1 = " << r1 << ", r2 = " << r2
                << ", Iteration: " << i << '\n';
  }

  // Should not get here (infinite loop)
  return 0;
}
