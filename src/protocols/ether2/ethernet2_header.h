#pragma once

#include "mac_addr.h"
#include "ethertype.h"

struct ethernet2_header {
    mac_addr src_addr;
    mac_addr dest_addr;

    ethertype prot;
};

int write_in_network_order(uint8_t* dest, ethernet2_header const* src);

int extract_from_network_order(ethernet2_header* dest, uint8_t const* src);

uint32_t ethernet_calc_crc(uint8_t const* data, size_t sz);