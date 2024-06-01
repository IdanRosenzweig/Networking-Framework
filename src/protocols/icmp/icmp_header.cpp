#include <netinet/in.h>
#include "icmp_header.h"

int write_in_network_order(uint8_t *dest, icmp_header *src) {
    *dest++ = src->type;
    *dest++ = src->code;

    *(uint16_t *) dest = htons(src->checksum);
    dest += sizeof(uint16_t);

    *(uint32_t *) dest = htonl(src->content.raw);
    dest += sizeof(uint32_t);

    return sizeof(icmp_header);
}

int extract_from_network_order(icmp_header *dest, uint8_t *src) {
    dest->type = *src++;
    dest->code = *src++;

    dest->checksum = ntohs(*(uint16_t *) src);
    src += sizeof(uint16_t);

    dest->content.raw = ntohl(*(uint32_t *) src);
    src += sizeof(uint32_t);

    return sizeof(icmp_header);
}