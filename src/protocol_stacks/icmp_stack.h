#ifndef NETWORKING_ICMP_STACK_H
#define NETWORKING_ICMP_STACK_H

#include "../protocols/ip4/ip4_protocol.h"
#include "../protocols/icmp/icmp_protocol.h"

struct icmp_protocol_stack {
    ip4_protocol ip_prot;
    icmp_protocol icmp_prot;
};

struct icmp_packet_stack {
    ip4_addr source_addr;
    ip4_addr dest_addr;
    decltype(icmp_header::type) type;
    decltype(icmp_header::code) code;
    recv_msg_t plain_data;
};

#endif //NETWORKING_ICMP_STACK_H
