#ifndef TASK_GENERATOR_H
#define TASK_GENERATOR_H

#include <ostream>
#include "process.h"

class TaskGenerator : public Process {
public:
    void execute();

    friend std::ostream& operator<<(std::ostream &os, const TaskGenerator &process);
};

#endif
