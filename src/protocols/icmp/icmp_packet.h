#ifndef SERVERCLIENT_ICMP_PACKET_H
#define SERVERCLIENT_ICMP_PACKET_H

#include <stdint.h>
#include <linux/icmp.h>

struct icmp_packet {
//    icmphdr header{}; // the icmp v6 header struct for echo is the same
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        uint32_t raw;
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo; // content for type echo
        struct {
            uint32_t gateway; // content for type redirect
        } redirect;
        struct {
            uint16_t __unused;
            uint16_t mtu;
        } dest_unreach; // content for type destination unreachable
    } content; // content varying for different types

};


#endif //SERVERCLIENT_ICMP_PACKET_H
