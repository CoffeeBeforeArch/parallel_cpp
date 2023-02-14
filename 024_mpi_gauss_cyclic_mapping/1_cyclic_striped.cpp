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
  const int dim = 1 << 12;
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

  // Performance gaussian elimination
  for (int row = 0; row < dim; row++) {
    // See if this process is responsible for the pivot calculation
    auto mapped_rank = row % num_tasks;

    // If the row is mapped to this rank...
    if (task_id == mapped_rank) {
      // Calculate the row in the local matrix
      auto local_row = row / num_tasks;

      // Get the value of the pivot
      auto pivot = m_chunk[local_row * dim + row];

      // Divide the rest of the row by the pivot
      for (int col = row; col < dim; col++) {
        m_chunk[local_row * dim + col] /= pivot;
      }

      // Send the pivot row to the other processes
      MPI_Bcast(m_chunk.get() + dim * local_row, dim, MPI_FLOAT, mapped_rank,
                MPI_COMM_WORLD);

      // Eliminate the for the local rows
      for (int elim_row = local_row + 1; elim_row < n_rows; elim_row++) {
        // Get the scaling factor for elimination
        auto scale = m_chunk[elim_row * dim + row];

        // Remove the pivot
        for (int col = row; col < dim; col++) {
          m_chunk[elim_row * dim + col] -=
              m_chunk[local_row * dim + col] * scale;
        }
      }
    } else {
      // Receive pivot row
      MPI_Bcast(pivot_row.get(), dim, MPI_FLOAT, mapped_rank, MPI_COMM_WORLD);

      auto elim_start = row / num_tasks + (mapped_rank > task_id) * 1;
      for (int elim_row = elim_start; elim_row < n_rows; elim_row++) {
        // Get the scaling factor for elimination
        auto scale = m_chunk[elim_row * dim + row];

        // Remove the pivot
        for (int col = row; col < dim; col++) {
          m_chunk[elim_row * dim + col] -= pivot_row[col] * scale;
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

  // Finish our MPI work
  MPI_Finalize();
  return 0;
}
