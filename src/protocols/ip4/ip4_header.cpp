#include <netinet/in.h>
#include "ip4_header.h"

int write_in_network_order(uint8_t * dest, ip4_header* src) {
    *dest++ = (src->version << 4) + src->header_len;
    *dest++ = src->type_of_service;

    *(uint16_t *)dest = htons(src->total_len);
    dest += sizeof(uint16_t);

    *(uint16_t *)dest = htons(src->id);
    dest += sizeof(uint16_t);

    *(uint16_t *)dest = htons((src->flags << 13) + src->frag_off);
    dest += sizeof(uint16_t);

    *dest++ = src->ttl;

    *dest++ = src->protocol;

    *(uint16_t *)dest = htons(src->checksum);
    dest += sizeof(uint16_t);

    dest += write_in_network_order(dest, &src->source_addr);
    dest += write_in_network_order(dest, &src->dest_addr);

    return sizeof(ip4_header);
}

int extract_from_network_order(ip4_header* dest, uint8_t* src) {
    dest->header_len = (*src) & 0b1111;
    dest->version = ((*src) & 0b11110000) >> 4;
    src++;

    dest->type_of_service = *src++;

    dest->total_len = ntohs(*(uint16_t*) src);
    src += sizeof(uint16_t);

    dest->id = ntohs(*(uint16_t*) src);
    src += sizeof(uint16_t);

    dest->frag_off = ntohs(*(uint16_t*) src) & ((uint16_t) 0b1111111111111);
    dest->flags = ntohs(*(uint16_t*) src) & ((uint16_t) 0b1110000000000000);
    src += sizeof(uint16_t);

    dest->ttl = *src++;

    dest->protocol = *src++;

    dest->checksum = ntohs(*(uint16_t*) src);
    src += sizeof(uint16_t);

    src += extract_from_network_order(&dest->source_addr, src);
    src += extract_from_network_order(&dest->dest_addr, src);

    return sizeof(ip4_header);
}