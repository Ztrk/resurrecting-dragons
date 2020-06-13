#include <iostream>
#include <mpi.h>
#include "packet.h"
#include "process.h"
#include "professional.h"
#include "task_generator.h"
using namespace std;

int main(int argc, char **argv) {
    const int HEADS_NUM = 2;
    const int BODY_NUM = 2;
    const int TAIL_NUM = 3;

    const int OFFICE_NUM = 3;
    const int SKELETON_NUM = 3;
    const int WORK_TIME = 2;

    const int TIME_PER_TASK = 2;

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
        cerr << "MPI does not provide enough thread support\n";
        cerr << "Requested: " << MPI_THREAD_MULTIPLE 
            << " provided: " << provided << '\n';
        cerr << "Exiting\n";
        MPI_Finalize();
        return 1;
    }

    Packet::initialize_datatype();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (HEADS_NUM + BODY_NUM + TAIL_NUM + 1 != size) {
        cerr << "Number of processes with specialization must be equal to number of process - 1" << endl;
        MPI_Finalize();
        return 1;
    }

    vector<Professional::Specialization> specializations;
    specializations.push_back(Professional::Specialization::UNDEFINED);
    for (int i = 0; i < HEADS_NUM; ++i) {
        specializations.push_back(Professional::Specialization::HEAD);
    }
    for (int i = 0; i < BODY_NUM; ++i) {
        specializations.push_back(Professional::Specialization::BODY);
    }
    for (int i = 0; i < TAIL_NUM; ++i) {
        specializations.push_back(Professional::Specialization::TAIL);
    }

    Process *process;
    if (rank == 0) {
        process = new TaskGenerator(TIME_PER_TASK);
    }
    else {
        process = new Professional(specializations, OFFICE_NUM, SKELETON_NUM, WORK_TIME);
    }

    process->execute();

    delete process;
    MPI_Finalize();

    return 0;
}
