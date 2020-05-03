#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H
#include <ostream>

#include "process.h"

class Professional : public Process {
public:
    void execute();

    friend std::ostream& operator<<(std::ostream &os, const Professional &process);

    enum class State {
        START, WAIT_TASK, HAS_TASK
    };
    enum class Specialization {
        HEAD, BODY, TAIL
    };
private:
    State state = State::START;
    Specialization spec = Specialization::HEAD;

    int tasks_produced = 0;
    int tasks_consumed = 0;
    int tasks_lower_priority = 0;

    int ack_count = 0;
    int request_priority = 0;

    bool has_higher_priority(int other_priority, int other_rank);
};

#endif
