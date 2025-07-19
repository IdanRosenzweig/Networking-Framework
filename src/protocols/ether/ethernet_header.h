#pragma once

#include "mac_addr.h"
#include "ethertype.h"

struct ethernet_header {
    mac_addr source_addr;
    mac_addr dest_addr;

    ethertype ether_type;
};

int write_in_network_order(uint8_t* dest, ethernet_header const* src);

int extract_from_network_order(ethernet_header* dest, uint8_t const* src);

uint32_t ethernet_calc_crc(uint8_t const* data, size_t sz);