#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <mpi.h>
#include "professional.h"
#include "packet.h"
#include "logging.h"
using namespace std;

Professional::Professional(vector<Specialization> specializations, 
    int office_num, int skeleton_num, int work_time)
: specialization(specializations[rank]),
specializations(specializations),
specialization_size(count(specializations.begin(), specializations.end(), specialization)), 
OFFICE_NUM(office_num), SKELETON_NUM(skeleton_num), WORK_TIME(work_time) { }

bool Professional::has_higher_priority(int other_priority, int other_rank) {
    return request_priority < other_priority
        || (request_priority == other_priority && rank < other_rank);
}

void Professional::execute() {
    MPI_Status status;
    Packet packet;

    INFO << *this << "Starting work\n";
    set_state(State::START);
    while (true) {
        MPI_Recv(&packet, 1, MPI_PACKET, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        update_clock(packet.time);

        handle_packet(packet, static_cast<PacketTag>(status.MPI_TAG), status.MPI_SOURCE);
    }
}

void Professional::set_state(State new_state) {
    state = new_state;
    on_change_state();
}

void Professional::handle_packet(Packet &packet, PacketTag tag, int source) {
    switch (tag) {
        case TASK:
            TRACE << *this << "Received TASK, id: " << packet.data << '\n';
            ++tasks_produced;
            if (state == State::HAS_TEAM && tasks_produced >= task_id) {
                set_state(State::IDLE);
            }
            break;
        case REQ_TASK:
            TRACE << *this << "Received REQ_TASK " << packet << '\n';
            if (state == State::WAIT_TASK && has_higher_priority(packet.data, source)) {
                ++tasks_lower_priority;
            }
            else {
                ++tasks_consumed;
            }
            send_packet(packet, source, ACK_TASK);
            break;
        case ACK_TASK:
            TRACE << *this << "Received ACK_TASK " << packet << '\n';
            handle_ack(packet.data, specialization_size - 1, State::HAS_TASK);
            break;
        case OFFER:
            TRACE << *this << "Received OFFER " << packet << '\n';
            if (offers.find(packet.data) == offers.end()) {
                offers[packet.data] = array<int, 3>{-1, -1, -1};
            }
            // scope for offer
            {
                auto &offer = offers[packet.data];
                offer[get_specialization(source)] = source;
                if (state == State::HAS_TASK && packet.data == task_id) {
                    int cnt = count(offer.begin(), offer.end(), -1);
                    if (cnt <= 1) {
                        set_state(State::HAS_TEAM);
                    }
                }
            }
            break;
        case REQ_OFFICE:
            TRACE << *this << "Received REQ_OFFICE " << packet << '\n';
            if (state == State::WORK_OFFICE 
                    || (state == State::WAIT_OFFICE && has_higher_priority(packet.data, source))) {
                requests.push_back(packet);
            }
            else {
                send_packet(packet, source, ACK_OFFICE);
            }
            break;
        case ACK_OFFICE:
            TRACE << *this << "Received ACK_OFFICE " << packet << '\n';
            if (state == State::WAIT_OFFICE) {
                handle_ack(packet.data, specialization_size - OFFICE_NUM, State::WORK_OFFICE);
            }
            break;
        case END_OFFICE:
            DEBUG << *this << "Received END_OFFICE " << packet << '\n';
            if (specialization == Specialization::BODY) {
                set_state(State::WAIT_SKELETON);
            }
            else {
                set_state(State::FINISH_OFFICE);
            }
            break;
        case REQ_SKELETON:
            TRACE << *this << "Received REQ_SKELETON " << packet << '\n';
            if (state == State::WORK_SKELETON 
                    || (state == State::WAIT_SKELETON && has_higher_priority(packet.data, source))) {
                requests.push_back(packet);
            }
            else {
                send_packet(packet, source, ACK_SKELETON);
            }
            break;
        case ACK_SKELETON:
            TRACE << *this << "Received ACK_SKELETON " << packet << '\n';
            if (state == State::WAIT_SKELETON) {
                handle_ack(packet.data, specialization_size - SKELETON_NUM, State::WORK_SKELETON);
            }
            break;
        case START_SKELETON:
            DEBUG << *this << "Received START_SKELETON " << packet << '\n';
            set_state(State::WORK_SKELETON);
            break;
        case END_SKELETON:
            DEBUG << *this << "Received END_SKELETON " << packet << '\n';
            if (specialization == Specialization::BODY) {
                ++end_skeleton_count;
                if (end_skeleton_count >= 3) {
                    set_state(State::FINISH_SKELETON);
                }
            }
            else {
                send_packet(packet, offers[task_id][Specialization::BODY], END_SKELETON);
                set_state(State::START);
            }
            break;
        default:
            WARN << *this << "Unknown tag " << packet << '\n';
    }
}

void Professional::on_change_state() {
    Packet packet;
    switch (state) {
        case State::START:
            set_state(State::WAIT_TASK);
            break;
        case State::WAIT_TASK:
            DEBUG << *this << "Requesting task\n";
            tasks_lower_priority = 0;
            if (specialization_size > 1) {
                send_request(REQ_TASK);
            }
            else {
                set_state(State::HAS_TASK);
            }
            break;
        case State::HAS_TASK:
            task_id = tasks_consumed + 1;
            tasks_consumed += tasks_lower_priority + 1;
            INFO << *this << "My task has id: " << task_id << ", sending offers\n";

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
            INFO << *this << "My team: " 
                << offers[task_id][0] << " "
                << offers[task_id][1] << " "
                << offers[task_id][2] << '\n';
            if (tasks_produced >= task_id) {
                set_state(State::IDLE);
            }
            break;
        case State::IDLE:
            DEBUG << *this << "Nothing to do\n";
            if (specialization == Specialization::TAIL) {
                set_state(State::WAIT_OFFICE);
            }
            break;
        case State::WAIT_OFFICE:
            DEBUG << *this << "Sending request for office\n";
            if (specialization_size > OFFICE_NUM) {
                send_request(REQ_OFFICE);
            }
            else {
                set_state(State::WORK_OFFICE);
            }
            break;
        case State::WORK_OFFICE:
            INFO << *this << "Working in office started\n";
            thread(&Professional::work_office, this).detach();
            break;
        case State::FINISH_OFFICE:
            DEBUG << *this << "Finished work in office, sending END_OFFICE\n";
            send_packet(packet, offers[task_id][Specialization::BODY], END_OFFICE);
            for (size_t i = 0; i < requests.size(); ++i) {
                send_packet(requests[i], requests[i].source, ACK_OFFICE);
            }
            requests.clear();
            break;
        case State::WAIT_SKELETON:
            DEBUG << *this << "Sending request for skeleton\n";
            if (specialization_size > SKELETON_NUM) {
                send_request(REQ_SKELETON);
            }
            else {
                set_state(State::WORK_SKELETON);
            }
            break;
        case State::WORK_SKELETON:
            INFO << *this << "Resurrecting dragon started\n";
            if (specialization == Specialization::BODY) {
                send_packet(packet, offers[task_id][Specialization::HEAD], START_SKELETON);
                send_packet(packet, offers[task_id][Specialization::TAIL], START_SKELETON);
            }
            thread(&Professional::work_skeleton, this).detach();
            break;
        case State::FINISH_SKELETON:
            end_skeleton_count = 0;
            for (size_t i = 0; i < requests.size(); ++i) {
                send_packet(requests[i], requests[i].source, ACK_SKELETON);
            }
            requests.clear();
            set_state(State::START);
            break;
        default:
            break;
    }
}

void Professional::send_request(PacketTag request_tag) {
    Packet packet;
    ack_count = 0;
    request_priority = clock + 1;
    packet.data = request_priority;
    for (int i = 0; i < size; ++i) {
        if (get_specialization(i) == specialization && i != rank) {
            send_packet(packet, i, request_tag);
        }
    }
}

void Professional::handle_ack(int priority, int ack_threshold, State new_state) {
    if (request_priority == priority) {
        ++ack_count;
        if (ack_count >= ack_threshold) {
            set_state(new_state);
        }
    }
}

void Professional::work_office() {
    this_thread::sleep_for(chrono::seconds(WORK_TIME));
    INFO << *this << "Work in office finished" << '\n';
    Packet packet;
    packet.source = rank;
    MPI_Send(&packet, 1, MPI_PACKET, rank, END_OFFICE, MPI_COMM_WORLD);
}

void Professional::work_skeleton() {
    this_thread::sleep_for(chrono::seconds(WORK_TIME));
    INFO << *this << "Resurecting dragon finished" << '\n';
    Packet packet;
    packet.source = rank;
    MPI_Send(&packet, 1, MPI_PACKET, rank, END_SKELETON, MPI_COMM_WORLD);
}

Professional::Specialization Professional::get_specialization(int rank) {
    return specializations[rank];
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
