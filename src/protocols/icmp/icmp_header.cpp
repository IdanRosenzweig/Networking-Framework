#include "icmp_header.h"

#include <netinet/in.h>

int write_in_network_order(uint8_t* dest, icmp_header const* src) {
    *dest++ = static_cast<uint8_t>(src->type);
    *dest++ = src->code;

    // *(uint16_t*)dest = htons(src->checksum);
    *(uint16_t*)dest = src->checksum;
    dest += sizeof(uint16_t);

    *(uint32_t*)dest = htonl(src->content.raw);
    dest += sizeof(uint32_t);

    return sizeof(icmp_header);
}

int extract_from_network_order(icmp_header* dest, uint8_t const* src) {
    dest->type = static_cast<icmp_type>(*src++);
    dest->code = *src++;

    // dest->checksum = ntohs(*(uint16_t*)src);
    dest->checksum = *(uint16_t*)src;
    src += sizeof(uint16_t);

    dest->content.raw = ntohl(*(uint32_t*)src);
    src += sizeof(uint32_t);

    return sizeof(icmp_header);
}