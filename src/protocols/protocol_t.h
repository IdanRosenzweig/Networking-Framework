#ifndef NETWORKING_PROTOCOL_T_H
#define NETWORKING_PROTOCOL_T_H

#include "../abstract/receiving/msg/recv_msg_t.h"

enum protocol_t : recv_msg_protocol_t {
    ETHERNET,
    ARP,
    IP4,
    UDP,
    TCP,
    ICMP,
    DNS,
    EMP
};

#include <string>

std::string convert_to_str(protocol_t prot);

protocol_t convert_str_to_protocol_t(const std::string& prot);

#endif //NETWORKING_PROTOCOL_T_H
