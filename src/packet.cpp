#include <cstddef>
#include <mpi.h>
#include "packet.h"

MPI_Datatype MPI_PACKET;

void Packet::initialize_datatype() {
    const int fieldsNum = 3;
    int blocklengths[fieldsNum] = {1, 1, 1};
    MPI_Aint offsets[fieldsNum] = {offsetof(Packet, time),
        offsetof(Packet, source), offsetof(Packet, data)};
    MPI_Datatype types[fieldsNum] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Type_create_struct(fieldsNum, blocklengths, offsets, types, &MPI_PACKET);
    MPI_Type_commit(&MPI_PACKET);
}

std::ostream& operator<<(std::ostream &os, const Packet &packet) {
    os << "from: " << packet.source << " data: " << packet.data;
    return os;
}
