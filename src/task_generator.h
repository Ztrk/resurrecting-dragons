#ifndef TASK_GENERATOR_H
#define TASK_GENERATOR_H

#include <ostream>
#include "process.h"

class TaskGenerator : public Process {
public:
    TaskGenerator(int time_per_task);

    void execute();

    const int TIME_PER_TASK;

    friend std::ostream& operator<<(std::ostream &os, const TaskGenerator &process);
};

#endif
