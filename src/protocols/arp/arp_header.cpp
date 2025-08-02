#include "arp_header.h"

#include <cstdio>

#include <netinet/in.h>

size_t write_in_network_order(uint8_t* dest, arp_header const* src) {
    uint8_t* dest_orig = dest;

    *(uint16_t*)dest = htons(src->hard_type);
    dest += sizeof(src->hard_type);
    *(uint16_t*)dest = htons(static_cast<uint16_t>(src->prot_type));
    dest += sizeof(src->prot_type);

    *(uint8_t*)dest = src->hard_addr_sz;
    dest += sizeof(src->hard_addr_sz);
    *(uint8_t*)dest = src->prot_addr_sz;
    dest += sizeof(src->prot_addr_sz);

    *(uint16_t*)dest = htons(static_cast<uint16_t>(src->op));
    dest += sizeof(src->op);

    if (src->sender_hard_addr.size() != src->hard_addr_sz) {puts("size doesn't match");abort();} // todo make this function return optional<size_t> instead of aborting
    memcpy(dest, src->sender_hard_addr.data(), src->hard_addr_sz);
    dest += src->hard_addr_sz;

    if (src->sender_prot_addr.size() != src->prot_addr_sz) {puts("size doesn't match");abort();} // todo make this function return optional<size_t> instead of aborting
    memcpy(dest, src->sender_prot_addr.data(), src->prot_addr_sz);
    dest += src->prot_addr_sz;

    if (src->target_hard_addr.size() != src->hard_addr_sz) {puts("size doesn't match");abort();} // todo make this function return optional<size_t> instead of aborting
    memcpy(dest, src->target_hard_addr.data(), src->hard_addr_sz);
    dest += src->hard_addr_sz;

    if (src->target_prot_addr.size() != src->prot_addr_sz) {puts("size doesn't match");abort();} // todo make this function return optional<size_t> instead of aborting
    memcpy(dest, src->target_prot_addr.data(), src->prot_addr_sz);
    dest += src->prot_addr_sz;

    return dest - dest_orig;
}

size_t extract_from_network_order(arp_header* dest, uint8_t const* src) {
    uint8_t const* src_orig = src;

    dest->hard_type = ntohs(*(uint16_t*)src);
    src += sizeof(dest->hard_type);
    dest->prot_type = static_cast<ethertype>(ntohs(*(uint16_t*)src));
    src += sizeof(dest->prot_type);

    dest->hard_addr_sz = *(uint8_t*)src;
    src += sizeof(dest->hard_addr_sz);
    dest->prot_addr_sz = *(uint8_t*)src;
    src += sizeof(dest->prot_addr_sz);

    dest->op = static_cast<arp_op_values>(ntohs(*(uint16_t*)src));
    src += sizeof(dest->op);

    dest->sender_hard_addr = vector<uint8_t>(src, src + dest->hard_addr_sz); // todo fix vuln here
    src += dest->hard_addr_sz;
    dest->sender_prot_addr = vector<uint8_t>(src, src + dest->prot_addr_sz); // todo fix vuln here
    src += dest->prot_addr_sz;

    dest->target_hard_addr = vector<uint8_t>(src, src + dest->hard_addr_sz); // todo fix vuln here
    src += dest->hard_addr_sz;
    dest->target_prot_addr = vector<uint8_t>(src, src + dest->prot_addr_sz); // todo fix vuln here
    src += dest->prot_addr_sz;

    return src - src_orig;
}