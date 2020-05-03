#include <iostream>
#include <mpi.h>
#include "professional.h"
#include "packet.h"
using namespace std;

Professional::Professional(Specialization specialization)
: specialization(specialization) { 
    switch (specialization) {
        case Specialization::HEAD:
            specialization_size = (size - 1) / 3;
            break;
        case Specialization::BODY:
            specialization_size = size / 3;
            break;
        case Specialization::TAIL:
            specialization_size = (size + 1) / 3;
            break;
    }
}

bool Professional::has_higher_priority(int other_priority, int other_rank) {
    return request_priority < other_priority
        || (request_priority == other_priority && rank < other_rank);
}

void Professional::execute() {
    MPI_Status status;
    Packet packet;

    cout << *this << "Working\n";
    while (true) {
        if (state == State::START) {
            request_priority = clock + 1;
            packet.data = request_priority;
            for (int i = 0; i < size; ++i) {
                if (has_specialization(i, specialization) && i != rank) {
                    send_packet(packet, i, REQ_TASK);
                }
            }
            state = State::WAIT_TASK;
            if (specialization_size - 2 <= 0) {
                state = State::HAS_TASK;
            }
        }

        MPI_Recv(&packet, 1, MPI_PACKET, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        update_clock(packet.time);

        cout << *this << "Received message " 
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
                send_packet(packet, status.MPI_SOURCE, ACK_TASK);
                break;
            case ACK_TASK:
                if (packet.data == request_priority) {
                    ++ack_count;
                    if (ack_count == specialization_size - 2) {
                        int task_id = tasks_consumed + 1;
                        tasks_consumed += tasks_lower_priority + 1;
                        tasks_lower_priority = 0;
                        state = State::HAS_TASK;
                        cout << *this << "Received a task with id: " << task_id << '\n';
                    }
                }
                break;
            default:
                cout << *this << "Unknown tag\n";
        }
    }
}

bool Professional::has_specialization(int rank, Specialization s) {
    switch (s) {
        case Specialization::HEAD:
            return rank % 3 == 0;
        case Specialization::BODY:
            return rank % 3 == 1;
        case Specialization::TAIL:
            return rank % 3 == 2;
        default:
            return false;
    }
}

std::ostream& operator<<(std::ostream &os, const Professional &process) {
    if (process.specialization == Professional::Specialization::HEAD) {
        os << "\033[31;1m" << "HEAD";
    }
    else if (process.specialization == Professional::Specialization::BODY) {
        os << "\033[32;1m" << "BODY";
    }
    else {
        os << "\033[36;1m" << "TAIL";
    }
    process.print(os);
    os << "\033[0m";
    return os;
}
