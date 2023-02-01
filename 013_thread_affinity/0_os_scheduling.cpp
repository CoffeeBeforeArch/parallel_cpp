// A simple example of OS scheduling
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <atomic>
#include <thread>

// Aligned type to avoid atomics on the same cache line
struct AlignedAtomic {
  alignas(64) std::atomic<int> val = 0;
};

void os_scheduler() {
  // Aligned atomics
  AlignedAtomic a;
  AlignedAtomic b;

  // Work function for our threads
  auto work = [](AlignedAtomic &atomic){
      for(int i = 0; i < (1 << 20); i++) atomic.val++;
  };

  // Create four threads and use lambda to launch work
  std::thread t0(work, std::ref(a));
  std::thread t1(work, std::ref(a));
  std::thread t2(work, std::ref(b));
  std::thread t3(work, std::ref(b));

  // Join the threads
  t0.join();
  t1.join();
  t2.join();
  t3.join();
}

// OS scheduling benchmark 
static void os_scheduling(benchmark::State& s) {
  while (s.KeepRunning()) {
    os_scheduler();
  }
}
BENCHMARK(os_scheduling)->UseRealTime()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
