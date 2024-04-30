#ifndef SERVERCLIENT_PROTOCOL_T_H
#define SERVERCLIENT_PROTOCOL_T_H

enum protocol_t {
    ETHERNET,
    ARP,
    IP4,
    UDP,
    TCP,
    ICMP,
    DNS
};

#include <string>

std::string to_str(protocol_t prot);


#endif //SERVERCLIENT_PROTOCOL_T_H
