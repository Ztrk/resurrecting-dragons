#include <iostream>
#include <mpi.h>
#include "process.h"
using namespace std;

Process::Process() {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
}

Process::~Process() {}
