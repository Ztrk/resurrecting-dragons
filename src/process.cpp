#include <iostream>
#include <mpi.h>
#include "process.h"
using namespace std;

Process::Process() {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
}

void Process::execute() {
    int data;
    cout << "Working\n";
    cout << "Rank: " << rank << '\n';
    cout << "Size: " << size << '\n';
    while (true) {
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, nullptr);
        cout << "Received task: " << data << '\n';
    }
}

Process::~Process() {}
