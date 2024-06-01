#include <netinet/in.h>
#include "ethernet_header.h"

int write_in_network_order(uint8_t * dest, ethernet_header* src) {
    dest += write_in_network_order(dest, &src->dest_addr);
    dest += write_in_network_order(dest, &src->source_addr);
    *(uint16_t*) dest = htons(src->ether_type);
    return sizeof(ethernet_header);
}

int extract_from_network_order(ethernet_header* dest, uint8_t* src) {
    src += extract_from_network_order(&dest->dest_addr, src);
    src += extract_from_network_order(&dest->source_addr, src);
    dest->ether_type = ntohs(*(uint16_t*) src);
    return sizeof(ethernet_header);
}