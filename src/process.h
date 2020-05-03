#ifndef PROCESS_H
#define PROCESS_H
#include <ostream>
#include "packet.h"

class Process {
public:
    Process();
    virtual void execute() = 0;
    void print(std::ostream &os) const;

    virtual ~Process();

    friend std::ostream& operator<<(std::ostream &os, const Process &process);
protected:
    int rank;
    int size;
    int clock = 0;

    void update_clock();
    void update_clock(int other_clock);
    void send_packet(Packet &packet, int destination, PacketTag tag);
};

#endif
