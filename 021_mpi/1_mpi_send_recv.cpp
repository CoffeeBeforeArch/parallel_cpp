// A simple example of sending/receiving messages with MPI
// By: Nick from CoffeeBeforeArch

#include <iostream>

#include "mpi.h"

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the task ID
  int task_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

  // Rank 0 coordinates work
  if (task_id == 0) {
    // Get the total number of tasks
    int num_tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    // Send an integer to each thread
    for (int i = 1; i < num_tasks; i++) {
      MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    // Receive and integer back from each thread
    for (int i = 1; i < num_tasks; i++) {
      int recv;
      MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      // Print the received value
      std::cout << "Received " << recv << " from rank " << i << '\n';
    }
  } else {
    // Receive an integer from rank 0
    int recv;
    MPI_Recv(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Square the value
    recv *= recv;

    // Send back the squared value
    MPI_Send(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // Finish our MPI work
  MPI_Finalize();
  return 0;
}
