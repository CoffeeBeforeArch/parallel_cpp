// An example parallel program using MPI
// By: Nick from CoffeeBeforeArch

#include "mpi.h"

#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the total number of tasks
  int num_tasks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

  // Get the task ID
  int task_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

  // Set up the string and print
  std::stringstream ss;
  ss << "Printing from task " << task_id << '/' << num_tasks << '\n';
  std::cout << ss.str();

  // Finish our MPI work
  MPI_Finalize();
  return 0;
}
