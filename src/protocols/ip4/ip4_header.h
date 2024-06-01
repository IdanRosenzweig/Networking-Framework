#ifndef NETWORKING_IP4_HEADER_H
#define NETWORKING_IP4_HEADER_H

#include "ip4_addr.h"
#include "../../protocols/ip4/internet_checksum.h"

struct ip4_header {
    uint8_t version : 4;
    uint8_t header_len : 4;
    uint8_t type_of_service;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_off : 13;
    uint8_t ttl;

    uint8_t protocol;
    enum protocol_values : decltype(protocol) {
        icmp = 1,
        tcp = 6,
        udp = 17
    };

    uint16_t checksum;
    ip4_addr source_addr;
    ip4_addr dest_addr;
};

int write_in_network_order(uint8_t* dest, ip4_header* src);

int extract_from_network_order(ip4_header* dest, uint8_t* src);

#endif //NETWORKING_IP4_HEADER_H
