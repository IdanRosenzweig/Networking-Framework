#ifndef NETWORKING_UDP_STACK_H
#define NETWORKING_UDP_STACK_H

#include "../protocols/ip4/ip4_protocol.h"
#include "../protocols/udp/udp_protocol.h"

struct udp_protocol_stack {
    ip4_protocol ip_prot;
    udp_protocol udp_prot;
};

struct udp_packet_stack {
    ip4_addr source_addr;
    ip4_addr dest_addr;
    uint16_t source_port;
    uint16_t dest_port;
    recv_msg_t plain_data;
};

#endif //NETWORKING_UDP_STACK_H
