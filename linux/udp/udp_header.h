#ifndef SERVERCLIENT_UDP_HEADER_H
#define SERVERCLIENT_UDP_HEADER_H

#include <stdint.h>

struct udp_header {
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
};

#endif //SERVERCLIENT_UDP_HEADER_H
