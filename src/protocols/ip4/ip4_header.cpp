#include "ip4_header.h"

#include <netinet/in.h>

#include "internet_checksum.h"

optional<string> ip_prot_to_str(ip_prot_values prot) {
    switch (prot) {
        case ip_prot_values::icmp: return "icmp";
        case ip_prot_values::udp: return "udp";
        case ip_prot_values::tcp: return "tcp";
        default: return {};
    }
}

optional<ip_prot_values> str_to_ip_prot(string const& str) {
    if (str == "icmp") return ip_prot_values::icmp;
    else if (str == "udp") return ip_prot_values::udp;
    else if (str == "tcp") return ip_prot_values::tcp;
    else return {};
}

int write_in_network_order(uint8_t* dest, ip4_header const* src) {
    *dest++ = (src->version << 4) + src->header_len;
    *dest++ = src->type_of_service;

    *(uint16_t *)dest = htons(src->total_len);
    dest += sizeof(uint16_t);

    *(uint16_t *)dest = htons(src->id);
    dest += sizeof(uint16_t);

    *(uint16_t *)dest = htons((src->flags << 13) + src->frag_off);
    dest += sizeof(uint16_t);

    *dest++ = src->ttl;

    *dest++ = src->prot;

    // *(uint16_t *)dest = htons(src->checksum);
    *(uint16_t *)dest = src->checksum;
    dest += sizeof(uint16_t);

    dest += write_in_network_order(dest, &src->src_addr);
    dest += write_in_network_order(dest, &src->dest_addr);

    return sizeof(ip4_header);
}

int extract_from_network_order(ip4_header* dest, uint8_t const* src) {
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

    dest->prot = static_cast<ip_prot_values>(*src++);

    // dest->checksum = ntohs(*(uint16_t*) src);
    dest->checksum = *(uint16_t*) src;
    src += sizeof(uint16_t);

    src += extract_from_network_order(&dest->src_addr, src);
    src += extract_from_network_order(&dest->dest_addr, src);

    return sizeof(ip4_header);
}