#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H
#include <array>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "process.h"

class Professional : public Process {
public:
    enum class Specialization;
    enum class State;

    Professional(Specialization specialization);
    void execute();
    void handle_packet(Packet &packet, PacketTag tag, int source);
    void on_change_state();
    void set_state(State new_state);

    void send_request(PacketTag request_tag);

    Specialization get_specialization(int rank);

    friend std::ostream& operator<<(std::ostream &os, const Professional &process);

    enum class Specialization {
        HEAD = 0, BODY = 1, TAIL = 2, UNDEFINED
    };

    enum class State {
        START, WAIT_TASK, HAS_TASK, HAS_TEAM,
        IDLE, WAIT_OFFICE, WORK_OFFICE,
        WAIT_SKELETON, WORK_SKELETON
    };
private:
    State state = State::START;
    Specialization specialization;

    const int OFFICE_NUM = 3;
    const int SKELETON_NUM = 3;

    int tasks_produced = 0;
    int tasks_consumed = 0;
    int tasks_lower_priority = 0;

    int task_id = 0;

    int specialization_size;

    int ack_count = 0;
    int request_priority = 0;

    std::unordered_map<int, std::array<int, 3>> offers;
    std::vector<Packet> req_office;

    bool has_higher_priority(int other_priority, int other_rank);
};

#endif
