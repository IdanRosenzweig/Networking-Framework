#ifndef SERVERCLIENT_PROTOCOL_T_H
#define SERVERCLIENT_PROTOCOL_T_H

enum protocol_t {
    ETHERNET,
    ARP,
    IP4,
    UDP,
    TCP,
    ICMP,
    DNS,
    BS_PORT
};

#include <string>

std::string convert_to_str(protocol_t prot);

protocol_t convert_str_to_protocol_t(const std::string& prot);

#endif //SERVERCLIENT_PROTOCOL_T_H
