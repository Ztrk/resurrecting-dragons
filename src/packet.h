#ifndef MESSAGE_H
#define MESSAGE_H

struct Packet {
    int time;
    int data;

    static void initialize_datatype();
};

enum PacketTag {
    TASK, REQ_TASK, ACK_TASK
};

extern MPI_Datatype MPI_PACKET;

#endif
