#ifndef NETWORKING_ETHERNET_HEADER_H
#define NETWORKING_ETHERNET_HEADER_H

#include "mac_addr.h"

struct ethernet_header {
    mac_addr source_addr;
    mac_addr dest_addr;

    uint16_t ether_type;
    enum ethertype_values : decltype(ether_type) {
        ip4 = 0x0800,
        arp = 0x0806,
    };
};

int write_in_network_order(uint8_t* dest, ethernet_header* src);

int extract_from_network_order(ethernet_header* dest, uint8_t* src);

#endif //NETWORKING_ETHERNET_HEADER_H
