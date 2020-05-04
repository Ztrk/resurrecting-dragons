#ifndef MESSAGE_H
#define MESSAGE_H

struct Packet {
    int time;
    int data;

    static void initialize_datatype();
};

enum PacketTag {
    TASK, REQ_TASK, ACK_TASK, OFFER, 
    REQ_OFFICE, ACK_OFFICE, END_OFFICE,
    REQ_SKELETON, ACK_SKELETON,
    START_SKELETON, END_SKELETON
};

extern MPI_Datatype MPI_PACKET;

#endif
