#include <iostream>
#include <mpi.h>
#include "process.h"
#include "task_generator.h"
using namespace std;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Process *process;
    if (rank == 0) {
        process = new TaskGenerator();
    }
    else {
        process = new Process();
    }

    process->execute();

    delete process;
    MPI_Finalize();

    return 0;
}
