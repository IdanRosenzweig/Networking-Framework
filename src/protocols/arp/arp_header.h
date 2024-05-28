#ifndef NETWORKING_ARP_HEADER_H
#define NETWORKING_ARP_HEADER_H

#include <stdint.h>
#include <memory>
#include <cstring>

struct arp_header {
    uint16_t hard_type; // type of hardware address (usually MAC)
    uint16_t prot_type; // type of protocol address (usually IP)

    uint8_t hard_addr_sz; // size of the hardware address
    uint8_t prot_addr_sz; // size of the protocol address

    uint16_t oper; // type of operation

    std::unique_ptr<uint8_t> sender_hard_addr;
    std::unique_ptr<uint8_t> sender_prot_addr;

    std::unique_ptr<uint8_t> target_hard_addr;
    std::unique_ptr<uint8_t> target_prot_addr;

    void write_to_buff(char *buff) {
        memcpy(buff, &hard_type, sizeof(hard_type));
        buff += sizeof(hard_type);
        memcpy(buff, &prot_type, sizeof(prot_type));
        buff += sizeof(prot_type);

        memcpy(buff, &hard_addr_sz, sizeof(hard_addr_sz));
        buff += sizeof(hard_addr_sz);
        memcpy(buff, &prot_addr_sz, sizeof(prot_addr_sz));
        buff += sizeof(prot_addr_sz);

        memcpy(buff, &oper, sizeof(oper));
        buff += sizeof(oper);

        memcpy(buff, sender_hard_addr.get(), hard_addr_sz);
        buff += hard_addr_sz;
        memcpy(buff, sender_prot_addr.get(), prot_addr_sz);
        buff += prot_addr_sz;

        memcpy(buff, target_hard_addr.get(), hard_addr_sz);
        buff += hard_addr_sz;
        memcpy(buff, target_prot_addr.get(), prot_addr_sz);
        buff += prot_addr_sz;
    }
};

#endif //NETWORKING_ARP_HEADER_H
