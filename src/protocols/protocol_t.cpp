#include "protocol_t.h"
#include <iostream>

std::string convert_to_str(protocol_t prot) {
    switch (prot) {
        case ETHERNET: return "ethernet";
        case ARP: return "net_arp";
        case IP4 : return "ip4";
        case UDP: return "udp";
        case TCP: return "tcp";
        case ICMP: return "icmp";
        case DNS: return "dns";
    }

    return "";
}

protocol_t convert_str_to_protocol_t(const std::string& prot) {
    if (prot == "ethernet") return ETHERNET;
    else if (prot == "arp") return ARP;
    else if (prot == "ip4") return IP4;
    else if (prot == "udp") return UDP;
    else if (prot == "tcp") return TCP;
    else if (prot == "icmp") return ICMP;
    else if (prot == "dns") return DNS;
    else {
        std::cerr << "invalid protocol" << std::endl;
        throw;
    }
}