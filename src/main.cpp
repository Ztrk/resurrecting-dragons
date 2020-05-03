#include <iostream>
#include <mpi.h>
#include "packet.h"
#include "process.h"
#include "professional.h"
#include "task_generator.h"
using namespace std;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    Packet::initialize_datatype();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Process *process;
    if (rank == 0) {
        process = new TaskGenerator();
    }
    else {
        if (rank % 3 == 0) {
            process = new Professional(Professional::Specialization::HEAD);
        }
        else if (rank % 3 == 1) {
            process = new Professional(Professional::Specialization::BODY);
        }
        else {
            process = new Professional(Professional::Specialization::TAIL);
        }
    }

    process->execute();

    delete process;
    MPI_Finalize();

    return 0;
}
