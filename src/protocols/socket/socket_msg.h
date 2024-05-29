#ifndef NETWORKING_SOCKET_MSG_H
#define NETWORKING_SOCKET_MSG_H

#include "../ip4/ip4_addr.h"
#include "../../abstract/receiving/msg/received_msg.h"

struct socket_msg { // represents msg to endpoint
    ip4_addr source_addr;
    uint16_t source_port;
    uint16_t dest_port;
    received_msg msg;
};

struct logical_internet_socket {
    ip4_addr source_addr;
    uint16_t source_port;
    uint16_t dest_port;
};

#endif //NETWORKING_SOCKET_MSG_H
