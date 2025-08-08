#include "udp_header.h"

#include <netinet/in.h>

int write_in_network_order(uint8_t* dest, udp_header const* src) {
    *(udp_port*) dest = htons(src->src_port);
    dest += sizeof(udp_port);
    *(udp_port*) dest = htons(src->dest_port);
    dest += sizeof(udp_port);

    *(uint16_t *) dest = htons(src->len);
    dest += sizeof(uint16_t);

    *(uint16_t *) dest = htons(src->checksum);
    dest += sizeof(uint16_t);

    return sizeof(udp_header);
}

int extract_from_network_order(udp_header* dest, uint8_t const* src) {
    dest->src_port = ntohs(*(udp_port*) src);
    src += sizeof(udp_port);
    dest->dest_port = ntohs(*(udp_port*) src);
    src += sizeof(udp_port);

    dest->len = ntohs(*(udp_port*) src);
    src += sizeof(uint16_t);

    dest->checksum = ntohs(*(udp_port*) src);
    src += sizeof(uint16_t);

    return sizeof(udp_header);
}