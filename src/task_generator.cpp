#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>
#include "task_generator.h"
using namespace std;

void TaskGenerator::execute() {
    int task_id = 1;
    cout << "Generating tasks\n";
    cout << "Rank: " << rank << '\n';
    cout << "Size: " << size << '\n';
    while (true) {
        cout << "Generated task: " << task_id << '\n';
        for (int i = 0; i < size; ++i) {
            if (i != rank) {
                MPI_Send(&task_id, 1, MPI_INT, i, 1001, MPI_COMM_WORLD);
            }
        }
        ++task_id;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
