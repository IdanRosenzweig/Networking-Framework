#include "protocol_t.h"

std::string to_str(protocol_t prot) {
    switch (prot) {
        case ETHERNET: return "ethernet";
        case ARP: return "arp";
        case IP4 : return "ip4";
        case UDP: return "udp_client_server";
        case TCP: return "tcp_client_server";
        case ICMP: return "icmp";
        case DNS: return "dns";
    }

    return "";
}