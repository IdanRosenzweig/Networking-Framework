#ifndef SERVERCLIENT_SOCKET_MSG_H
#define SERVERCLIENT_SOCKET_MSG_H

#include "../ip4/ip4_addr.h"
#include "../../abstract/receiving/msg/received_msg.h"

struct socket_msg { // represents msg to endpoint
    ip4_addr source_addr;
    int source_port;
    int dest_port;
    received_msg msg;
};

struct logical_internet_socket {
    ip4_addr source_addr;
    int source_port;
    int dest_port;
};

#endif //SERVERCLIENT_SOCKET_MSG_H
