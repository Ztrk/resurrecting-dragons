#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>
#include "task_generator.h"
#include "packet.h"
#include "logging.h"
using namespace std;

TaskGenerator::TaskGenerator(int time_per_task) 
    : TIME_PER_TASK(time_per_task) { }

void TaskGenerator::execute() {
    Packet packet;
    int task_id = 1;
    INFO << *this << "Starting task generator\n";
    while (true) {
        update_clock();
        INFO << *this << "Generated task: " << task_id << '\n';
        packet.data = task_id;
        for (int i = 0; i < size; ++i) {
            if (i != rank) {
                send_packet(packet, i, TASK);
            }
        }
        ++task_id;
        std::this_thread::sleep_for(std::chrono::seconds(TIME_PER_TASK));
    }
}

std::ostream& operator<<(std::ostream &os, const TaskGenerator &process) {
    os << "\033[33;1m" << "TGEN";
    process.print(os);
    os << "\033[0m";
    return os;
}
