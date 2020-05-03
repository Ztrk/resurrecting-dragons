#include <iostream>
#include <mpi.h>
#include "professional.h"
#include "packet.h"
using namespace std;

bool Professional::has_higher_priority(int other_priority, int other_rank) {
    return request_priority < other_priority
        || (request_priority == other_priority && rank < other_rank);
}

void Professional::execute() {
    MPI_Status status;
    Packet packet;
    cout << "Working\n";
    cout << "Rank: " << rank << '\n';
    cout << "Size: " << size << '\n';
    while (true) {
        if (state == State::START) {
            request_priority = 0;
            packet.data = request_priority;
            for (int i = 0; i < size; ++i) {
                if (i != rank) {
                    MPI_Send(&packet, 1, MPI_PACKET, i, REQ_TASK, MPI_COMM_WORLD);
                }
            }
            state = State::WAIT_TASK;
        }
        MPI_Recv(&packet, 1, MPI_PACKET, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        cout << "Received message " 
            << "TAG: " << status.MPI_TAG
            << " FROM: " << status.MPI_SOURCE
            << " data: " << packet.data << '\n';
        switch (status.MPI_TAG) {
            case TASK:
                ++tasks_produced;
                break;
            case REQ_TASK:
                if (state == State::WAIT_TASK) {
                    if (has_higher_priority(packet.data, status.MPI_SOURCE)) {
                        ++tasks_lower_priority;
                    }
                    else {
                        ++tasks_consumed;
                    }
                }
                else {
                    ++tasks_consumed;
                }
                MPI_Send(&packet, 1, MPI_PACKET, status.MPI_SOURCE, ACK_TASK, MPI_COMM_WORLD);
                break;
            case ACK_TASK:
                if (packet.data == request_priority) {
                    ++ack_count;
                    if (ack_count == size - 2) {
                        int task_id = tasks_consumed + 1;
                        tasks_consumed += tasks_lower_priority + 1;
                        tasks_lower_priority = 0;
                        state = State::HAS_TASK;
                        cout << "We received a task with id: " << task_id << '\n';
                    }
                }
                break;
            default:
                cout << "Unknown tag\n";
        }
    }
}
