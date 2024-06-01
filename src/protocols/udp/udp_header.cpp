#include <netinet/in.h>
#include "udp_header.h"

int write_in_network_order(uint8_t* dest, udp_header* src) {
    *(udp_port*) dest = htons(src->source_port);
    dest += sizeof(udp_port);
    *(udp_port*) dest = htons(src->dest_port);
    dest += sizeof(udp_port);

    *(uint16_t *) dest = htons(src->len);
    dest += sizeof(uint16_t);

    *(uint16_t *) dest = htons(src->checksum);
    dest += sizeof(uint16_t);

    return sizeof(udp_header);
}

int extract_from_network_order(udp_header* dest, uint8_t* src) {
    dest->source_port = ntohs(*(udp_port*) src);
    src += sizeof(udp_port);
    dest->dest_port = ntohs(*(udp_port*) src);
    src += sizeof(udp_port);

    dest->len = ntohs(*(udp_port*) src);
    src += sizeof(uint16_t);

    dest->checksum = ntohs(*(udp_port*) src);
    src += sizeof(uint16_t);

    return sizeof(udp_header);
}