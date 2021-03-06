#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H
#include <array>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "process.h"

class Professional : public Process {
public:
    enum Specialization {
        HEAD = 0, BODY = 1, TAIL = 2, UNDEFINED
    };
    enum class State;

    Professional(std::vector<Specialization> specializations, int office_num, int skeleton_num, int work_time);

    void execute();
    void handle_packet(Packet &packet, PacketTag tag, int source);
    void on_change_state();
    void set_state(State new_state);

    void send_request(PacketTag request_tag);
    void handle_ack(int priority, int ack_threshold, State new_state);

    void work_office();
    void work_skeleton();

    Specialization get_specialization(int rank);

    friend std::ostream& operator<<(std::ostream &os, const Professional &process);

    enum class State {
        START, WAIT_TASK, HAS_TASK, HAS_TEAM,
        IDLE, WAIT_OFFICE, WORK_OFFICE, FINISH_OFFICE,
        WAIT_SKELETON, WORK_SKELETON, FINISH_SKELETON
    };
private:
    State state = State::START;

    Specialization specialization;
    std::vector<Specialization> specializations;
    int specialization_size;

    const int OFFICE_NUM;
    const int SKELETON_NUM;
    const int WORK_TIME;

    int tasks_produced = 0;
    int tasks_consumed = 0;
    int tasks_lower_priority = 0;

    int task_id = 0;

    int ack_count = 0;
    int request_priority = 0;

    std::unordered_map<int, std::array<int, 3>> offers;
    std::vector<Packet> requests;

    int end_skeleton_count = 0;

    bool has_higher_priority(int other_priority, int other_rank);
};

#endif
