#ifndef NETWORKING_UDP_HEADER_H
#define NETWORKING_UDP_HEADER_H

#include <stdint.h>

using udp_port = uint16_t;

struct udp_header {
    udp_port source_port;
    udp_port dest_port;
    uint16_t len;
    uint16_t checksum;
};

int write_in_network_order(uint8_t* dest, udp_header* src);

int extract_from_network_order(udp_header* dest, uint8_t* src);

#endif //NETWORKING_UDP_HEADER_H
