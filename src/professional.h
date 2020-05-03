#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H
#include <ostream>

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

    bool has_specialization(int rank, Specialization s);

    friend std::ostream& operator<<(std::ostream &os, const Professional &process);

    enum class Specialization {
        HEAD, BODY, TAIL
    };

    enum class State {
        START, WAIT_TASK, HAS_TASK
    };
private:
    State state = State::START;
    Specialization specialization;

    int tasks_produced = 0;
    int tasks_consumed = 0;
    int tasks_lower_priority = 0;

    int specialization_size;

    int ack_count = 0;
    int request_priority = 0;

    bool has_higher_priority(int other_priority, int other_rank);
};

#endif
