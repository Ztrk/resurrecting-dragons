#include <algorithm>
#include <iostream>
#include <mpi.h>
#include "process.h"
#include "packet.h"
using namespace std;

Process::Process() {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
}

Process::~Process() {}

void Process::update_clock() {
    ++clock;
}

void Process::update_clock(int other_clock) {
    clock = max(clock, other_clock) + 1;
}

void Process::send_packet(Packet &packet, int destination, PacketTag tag) {
    update_clock();
    packet.time = clock;
    MPI_Send(&packet, 1, MPI_PACKET, destination, tag, MPI_COMM_WORLD);
}
