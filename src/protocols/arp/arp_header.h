#pragma once

#include <stdint.h>
#include <cstring>
#include <vector>
using namespace std;

#include <netinet/in.h>

struct arp_header {
    uint16_t hard_type; // type of hardware address
    uint16_t prot_type; // type of protocol address

    uint8_t hard_addr_sz; // size of the hardware address
    uint8_t prot_addr_sz; // size of the protocol address

    uint16_t op; // type of operation

    vector<uint8_t> sender_hard_addr;
    vector<uint8_t> sender_prot_addr;

    vector<uint8_t> target_hard_addr;
    vector<uint8_t> target_prot_addr;

    void write_to_buff(uint8_t* buff) {
        *(uint16_t*)buff = htons(hard_type);
        buff += sizeof(hard_type);
        *(uint16_t*)buff = htons(prot_type);
        buff += sizeof(prot_type);

        *(uint8_t*)buff = hard_addr_sz;
        buff += sizeof(hard_addr_sz);
        *(uint8_t*)buff = prot_addr_sz;
        buff += sizeof(prot_addr_sz);

        *(uint16_t*)buff = htons(op);
        buff += sizeof(op);

        memcpy(buff, sender_hard_addr.data(), hard_addr_sz);
        buff += hard_addr_sz;
        memcpy(buff, sender_prot_addr.data(), prot_addr_sz);
        buff += prot_addr_sz;

        memcpy(buff, target_hard_addr.data(), hard_addr_sz);
        buff += hard_addr_sz;
        memcpy(buff, target_prot_addr.data(), prot_addr_sz);
        buff += prot_addr_sz;
    }

    size_t calc_sz() {
        return
            sizeof(hard_type)
            + sizeof(prot_type)
            + sizeof(hard_addr_sz)
            + sizeof(prot_addr_sz)
            + sizeof(op)
            + sender_hard_addr.size()
            + sender_prot_addr.size()
            + target_hard_addr.size()
            + target_prot_addr.size()
        ;
    }
};

size_t write_in_network_order(uint8_t* dest, arp_header const* src);

size_t extract_from_network_order(arp_header* dest, uint8_t const* src);
