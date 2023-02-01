// This program shows off setting thread affinity
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>
#include <pthread.h>

#include <atomic>
#include <cassert>
#include <thread>

// Aligned type to avoid atomics on the same cache line
struct AlignedAtomic {
  alignas(64) std::atomic<int> val = 0;
};

void thread_affinity() {
  AlignedAtomic a;
  AlignedAtomic b;

  // Create cpu sets for threads 0,1 and 2,3
  cpu_set_t cpu_set_0;
  cpu_set_t cpu_set_1;

  // Zero them out
  CPU_ZERO(&cpu_set_0);
  CPU_ZERO(&cpu_set_1);

  // And set the CPU cores we want to pin the threads too
  CPU_SET(0, &cpu_set_0);
  CPU_SET(1, &cpu_set_1);

  // Work function for our threads
  auto work = [](AlignedAtomic &atomic){
      for(int i = 0; i < (1 << 20); i++) atomic.val++;
  };
  
  // Create thread 0 and 1, and pin them to core 0
  std::thread t0(work, std::ref(a));
  std::thread t1(work, std::ref(a));
  assert(pthread_setaffinity_np(t0.native_handle(), sizeof(cpu_set_t),
                                &cpu_set_0) == 0);
  assert(pthread_setaffinity_np(t1.native_handle(), sizeof(cpu_set_t),
                                &cpu_set_0) == 0);

  // Create thread 1 and 2, and pin them to core 1
  std::thread t2(work, std::ref(b));
  std::thread t3(work, std::ref(b));
  assert(pthread_setaffinity_np(t2.native_handle(), sizeof(cpu_set_t),
                                &cpu_set_1) == 0);
  assert(pthread_setaffinity_np(t3.native_handle(), sizeof(cpu_set_t),
                                &cpu_set_1) == 0);

  // Join the threads
  t0.join();
  t1.join();
  t2.join();
  t3.join();
}

// Thread affinity benchmark
static void thread_affinity(benchmark::State& s) {
  for(auto _ : s) {
    thread_affinity();
  }
}
BENCHMARK(thread_affinity)->UseRealTime()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
