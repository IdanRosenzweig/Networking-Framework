#ifndef SERVERCLIENT_ICMP_PACKET_H
#define SERVERCLIENT_ICMP_PACKET_H

#include <stdint.h>

struct icmp_packet {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union content {
        uint32_t raw;
        struct echo_content {
            uint16_t id;
            uint16_t sequence;
        } echo; // content for type echo
        struct redirect_content {
            uint32_t gateway; // content for type redirect
        } redirect;
        struct dest_unreachable_content {
            uint16_t __unused;
            uint16_t mtu;
        } dest_unreach; // content for type destination unreachable
    } content; // content varying for different types

};


#endif //SERVERCLIENT_ICMP_PACKET_H
