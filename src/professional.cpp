#include <algorithm>
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
            specialization_size = (size + 1) / 3;
            break;
        case Specialization::TAIL:
            specialization_size = size / 3;
            break;
        default:
            specialization_size = size;
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
    set_state(State::START);
    while (true) {
        MPI_Recv(&packet, 1, MPI_PACKET, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        update_clock(packet.time);

        handle_packet(packet, static_cast<PacketTag>(status.MPI_TAG), status.MPI_SOURCE);
    }
}


void Professional::handle_packet(Packet &packet, PacketTag tag, int source) {
    cout << *this << "Received message " 
        << "TAG: " << tag
        << " FROM: " << source
        << " data: " << packet.data << '\n';
    switch (tag) {
        case TASK:
            ++tasks_produced;
            if (state == State::HAS_TEAM && tasks_produced >= task_id) {
                set_state(State::IDLE);
            }
            break;
        case REQ_TASK:
            if (state == State::WAIT_TASK) {
                if (has_higher_priority(packet.data, source)) {
                    ++tasks_lower_priority;
                }
                else {
                    ++tasks_consumed;
                }
            }
            else {
                ++tasks_consumed;
            }
            send_packet(packet, source, ACK_TASK);
            break;
        case ACK_TASK:
            if (packet.data == request_priority) {
                ++ack_count;
                if (ack_count == specialization_size - 2) {
                    set_state(State::HAS_TASK);
                }
            }
            break;
        case OFFER:
            if (offers.find(packet.data) == offers.end()) {
                offers[packet.data] = array<int, 3>{-1, -1, -1};
            }
            {
                // scope for offer
                auto &offer = offers[packet.data];
                offer[static_cast<int>(get_specialization(source))] = source;
                if (state == State::HAS_TASK && packet.data == task_id) {
                    int cnt = count(offer.begin(), offer.end(), -1);
                    if (cnt <= 1) {
                        set_state(State::HAS_TEAM);
                    }
                }
            }
            break;
        default:
            cout << *this << "Unknown tag\n";
    }
}

void Professional::on_change_state() {
    Packet packet;
    switch (state) {
        case State::START:
            set_state(State::WAIT_TASK);
            break;
        case State::WAIT_TASK:
            ack_count = 0;
            tasks_lower_priority = 0;
            request_priority = clock + 1;

            packet.data = request_priority;
            for (int i = 0; i < size; ++i) {
                if (get_specialization(i) == specialization && i != rank) {
                    send_packet(packet, i, REQ_TASK);
                }
            }

            if (specialization_size - 2 <= 0) {
                set_state(State::HAS_TASK);
            }
            break;
        case State::HAS_TASK:
            task_id = tasks_consumed + 1;
            tasks_consumed += tasks_lower_priority + 1;
            cout << *this << "Received a task with id: " << task_id << '\n';

            packet.data = task_id;
            for (int i = 0; i < size; ++i) {
                if (get_specialization(i) != specialization && i != 0) {
                    send_packet(packet, i, OFFER);
                }
            }
            if (offers.find(task_id) == offers.end()) {
                offers[task_id] = array<int, 3>{-1, -1, -1};
            }
            else {
                auto &offer = offers[task_id];
                int cnt = count(offer.begin(), offer.end(), -1);
                if (cnt <= 1) {
                    set_state(State::HAS_TEAM);
                }
            }
            break;
        case State::HAS_TEAM:
            cout << *this << "We are in team\n";
            if (tasks_produced >= task_id) {
                set_state(State::IDLE);
            }
            break;
        case State::IDLE:
            cout << *this << "Nothing to do\n";
            break;
        default:
            break;
    }
}

void Professional::set_state(State new_state) {
    state = new_state;
    on_change_state();
}

Professional::Specialization Professional::get_specialization(int rank) {
    if (rank == 0) {
        return Specialization::UNDEFINED;
    }
    switch (rank % 3) {
        case 0:
            return Specialization::HEAD;
        case 1:
            return Specialization::BODY;
        case 2:
            return Specialization::TAIL;
        default:
            return Specialization::UNDEFINED;
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
