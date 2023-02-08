// An example of sum reduction using MPI
// By: Nick from CoffeeBeforeArch

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>

#include "mpi.h"

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the total number of tasks
  int num_tasks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

  // Calculate chunk size
  // Assume this divides evenly
  const int num_elements = 1 << 10;
  const int chunk_size = num_elements / num_tasks;

  // Get the task ID
  int task_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

  // Create buffer for send (only initialized in rank 0)
  int *send_buffer;

  // Generate random numbers from rank 0
  if (task_id == 0) {
    // Allocate memory for send buffer
    send_buffer = new int[1024];

    // Create random number generator
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dist(1, 10);

    // Create random data
    std::generate(send_buffer, send_buffer + num_elements,
                  [&] { return dist(mt); });
  }

  // Receive buffer
  auto recv_buffer = std::make_unique<int[]>(chunk_size);

  // Perform the scatter of the data to different threads
  MPI_Scatter(send_buffer, chunk_size, MPI_INT, recv_buffer.get(), chunk_size,
              MPI_INT, 0, MPI_COMM_WORLD);

  // Calculate partial results in each thread
  auto local_result =
      std::reduce(recv_buffer.get(), recv_buffer.get() + chunk_size);

  // Perform the reduction
  int global_result;
  MPI_Reduce(&local_result, &global_result, 1, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Print the result from rank 0
  // Free the allocated memory
  if (task_id == 0) {
    std::cout << global_result << '\n';
    delete [] send_buffer;
  }

  // Finish our MPI work
  MPI_Finalize();
  return 0;
}
