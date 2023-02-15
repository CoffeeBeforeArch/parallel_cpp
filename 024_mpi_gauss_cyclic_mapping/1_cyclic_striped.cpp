// MPI parallel gaussian elimination
// By: Nick from CoffeeBeforeArch

#include <algorithm>
#include <memory>
#include <random>

#include "mpi.h"
#include <iostream>

void print_matrix(const float *matrix, int dim) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      std::cout << matrix[i * dim + j] << ' ';
    }
    std::cout << '\n';
  }
}

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the total number of tasks
  int num_tasks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

  // Calculate the number of rows mapped to each process
  // Assumes this divides evenly
  const int dim = 1 << 4;
  const int n_rows = dim / num_tasks;

  // Get the task ID
  int task_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

  // Matrix - Only initialized in rank 0
  std::unique_ptr<float[]> matrix;

  // Each process will store a chunk of the matrix
  auto m_chunk = std::make_unique<float[]>(dim * n_rows);

  // Each process will receive a pivot row each iteration
  auto pivot_row = std::make_unique<float[]>(dim);

  // Only rank 0 create/initializes the matrix
  if (task_id == 0) {
    // Create a random number generator
    std::mt19937 mt(123);
    std::uniform_real_distribution dist(1.0f, 2.0f);

    // Create a matrix
    matrix = std::make_unique<float[]>(dim * dim);
    std::generate(matrix.get(), matrix.get() + dim * dim,
                  [&] { return dist(mt); });
  }

  // Before doing anything, send parts of the matrix to each process
  // Scatter in smaller chunks to equalize the work better
  for (int i = 0; i < n_rows; i++) {
    MPI_Scatter(matrix.get() + i * dim * num_tasks, dim, MPI_FLOAT,
                m_chunk.get() + i * dim, dim, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }

  // Perform gaussian elimination
  // For every row in the local matrix chunk...
  for (int row = 0; row < n_rows; row++) {
    // For every rank in the communicator
    for (int rank = 0; rank < num_tasks; rank++) {
      // Calculate the global column of the pivot
      auto global_col = row * num_tasks + rank;

      // If this row belongs to this rank...
      if (task_id == rank) {
        // Do the pivot calculation
        auto pivot = m_chunk[row * dim + global_col];
        for (int col = global_col; col < dim; col++) {
          m_chunk[row * dim + col] /= pivot;
        }

        // Send the pivot row to the other ranks
        MPI_Bcast(m_chunk.get() + row * dim, dim, MPI_FLOAT, rank,
                  MPI_COMM_WORLD);

        // Eliminate the for the local rows
        for (int elim_row = row + 1; elim_row < n_rows; elim_row++) {
          // Get the scaling factor for elimination
          auto scale = m_chunk[elim_row * dim + global_col];

          // Remove the pivot
          for (int col = global_col; col < dim; col++) {
            m_chunk[elim_row * dim + col] -= m_chunk[row * dim + col] * scale;
          }
        }
      } else {
        // Receive the pivot from the sending rank
        MPI_Bcast(pivot_row.get(), dim, MPI_FLOAT, rank, MPI_COMM_WORLD);

        // Get the starting position
        auto local_start = (task_id < rank) ? row + 1 : row;

        // Eliminate for the local rows
        for (int elim_row = local_start; elim_row < n_rows; elim_row++) {
          // Get the scaling factor for elimination
          auto scale = m_chunk[elim_row * dim + global_col];

          // Remove the pivot
          for (int col = global_col; col < dim; col++) {
            m_chunk[elim_row * dim + col] -= pivot_row[col] * scale;
          }
        }
      }
    }
  }

  // Gather the final results into rank 0
  // Gather piece-by-piece since rows are cyclically striped
  for (int i = 0; i < n_rows; i++) {
    MPI_Gather(m_chunk.get() + i * dim, dim, MPI_FLOAT,
               matrix.get() + num_tasks * i * dim, dim, MPI_FLOAT, 0,
               MPI_COMM_WORLD);
  }

  // Print result from rank 0
  if (task_id == 0) {
    print_matrix(matrix.get(), dim);
  }

  // Finish our MPI work
  MPI_Finalize();
  return 0;
}
