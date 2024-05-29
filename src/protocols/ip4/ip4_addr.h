#ifndef NETWORKING_IP4_ADDR_H
#define NETWORKING_IP4_ADDR_H

#include <stdint.h>
#include <string>
using namespace std;

struct ip4_addr {
    uint8_t octets[4];

    bool operator==(const ip4_addr &rhs) const;

    bool operator!=(const ip4_addr &rhs) const;

    bool operator<(const ip4_addr &rhs) const;

    bool operator>(const ip4_addr &rhs) const;

    bool operator<=(const ip4_addr &rhs) const;

    bool operator>=(const ip4_addr &rhs) const;
};

#define empty_ip4_addr ip4_addr{{0,0,0,0}}

ip4_addr generate_next_ip(ip4_addr addr);

ip4_addr convert_to_ip4_addr(const string &str);

string convert_to_str(ip4_addr ip_addr);

void write_in_network_order(uint8_t* dest, ip4_addr* source);

void extract_from_network_order(ip4_addr* dest, uint8_t* source);


struct ip4_subnet_mask {
    ip4_addr base_addr;
    int no_mask_bits; // the number of bits corresponding to the network mask
};

ip4_subnet_mask convert_to_ip4_subnet_mask(const string &str);

string convert_to_str(ip4_subnet_mask mask);

ip4_addr smallest_ip_addr_in_subnet(ip4_subnet_mask subnet);

bool is_inside_subnet(ip4_subnet_mask subnet, ip4_addr ip);


#endif //NETWORKING_IP4_ADDR_H
