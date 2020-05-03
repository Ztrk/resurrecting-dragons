#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>
#include "task_generator.h"
#include "packet.h"
using namespace std;

void TaskGenerator::execute() {
    Packet packet;
    int task_id = 1;
    cout << "Generating tasks\n";
    cout << "Rank: " << rank << '\n';
    cout << "Size: " << size << '\n';
    while (true) {
        update_clock();
        cout << "Generated task: " << task_id << '\n';
        packet.data = task_id;
        for (int i = 0; i < size; ++i) {
            if (i != rank) {
                send_packet(packet, i, TASK);
            }
        }
        ++task_id;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
