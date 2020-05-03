#ifndef PROCESS_H
#define PROCESS_H

class Process {
public:
    Process();
    virtual void execute() = 0;

    virtual ~Process();
protected:
    int rank;
    int size;
};

#endif
