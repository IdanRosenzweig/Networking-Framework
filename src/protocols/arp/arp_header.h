#pragma once

#include <stdint.h>
#include <cstring>
#include <vector>
#include <cstdlib>
using namespace std;

#include "arp_op_values.h"
#include "src/protocols/ether2/ethertype.h"

struct arp_header {
    uint16_t hard_type; // type of hardware address
    ethertype prot_type; // type of protocol address

    uint8_t hard_addr_sz; // size of the hardware address
    uint8_t prot_addr_sz; // size of the protocol address

    arp_op_values op; // type of operation

    vector<uint8_t> sender_hard_addr;
    vector<uint8_t> sender_prot_addr;

    vector<uint8_t> target_hard_addr;
    vector<uint8_t> target_prot_addr;

    size_t calc_sz() {
        return
            sizeof(hard_type)
            + sizeof(prot_type)
            + sizeof(hard_addr_sz)
            + sizeof(prot_addr_sz)
            + sizeof(op)
            + hard_addr_sz
            + prot_addr_sz
            + hard_addr_sz
            + prot_addr_sz
        ;
    }
};

size_t write_in_network_order(uint8_t* dest, arp_header const* src);

size_t extract_from_network_order(arp_header* dest, uint8_t const* src);
