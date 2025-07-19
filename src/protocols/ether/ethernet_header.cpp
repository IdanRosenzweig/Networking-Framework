#include <netinet/in.h>
#include "ethernet_header.h"

int write_in_network_order(uint8_t * dest, ethernet_header const* src) {
    dest += write_in_network_order(dest, &src->dest_addr);
    dest += write_in_network_order(dest, &src->source_addr);
    *(uint16_t*) dest = htons(src->ether_type);
    return sizeof(ethernet_header);
}

int extract_from_network_order(ethernet_header* dest, uint8_t const* src) {
    src += extract_from_network_order(&dest->dest_addr, src);
    src += extract_from_network_order(&dest->source_addr, src);
    dest->ether_type = static_cast<ethertype>(ntohs(*(uint16_t*) src));
    return sizeof(ethernet_header);
}


static const uint32_t POLY = 0xEDB88320;
static uint32_t crc32_table[256];
void init_crc32_table() {
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j)
            crc = (crc & 1) ? (POLY ^ (crc >> 1)) : (crc >> 1);
        crc32_table[i] = crc;
    }
}
uint32_t ethernet_calc_crc(uint8_t const* data, size_t sz) {
    bool init = false;
    if (!init) {
        init_crc32_table();
        init = true;
    }

    uint32_t crc = 0xFFFFFFFF;

    for (size_t off = 0; off < sz; off++) {
        uint8_t index = (crc ^ data[off]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }

    return ~crc;
}