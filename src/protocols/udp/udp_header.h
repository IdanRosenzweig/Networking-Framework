#ifndef NETWORKING_UDP_HEADER_H
#define NETWORKING_UDP_HEADER_H

#include <stdint.h>

struct udp_header {
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
};

#endif //NETWORKING_UDP_HEADER_H
