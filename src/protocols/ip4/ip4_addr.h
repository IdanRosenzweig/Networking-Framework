#pragma once

#include <cstdint>
#include <string>
#include <optional>
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

#define ip4_addr_empty (ip4_addr{{0,0,0,0}})

ip4_addr generate_next_ip(ip4_addr addr);

optional<ip4_addr> str_to_ip4_addr(string const& str);

string ip4_addr_to_str(ip4_addr const& ip_addr);

int write_in_network_order(uint8_t* dest, ip4_addr const* source);

int extract_from_network_order(ip4_addr* dest, uint8_t const* source);


struct ip4_subnet_mask {
    ip4_addr base_addr;
    int no_mask_bits; // the number of bits corresponding to the network mask
};

optional<ip4_subnet_mask> str_to_ip4_subnet_mask(string const& str);

string ip4_subnet_mask_to_str(ip4_subnet_mask mask);

ip4_addr smallest_ip_addr_in_subnet(ip4_subnet_mask subnet);

bool is_inside_subnet(ip4_subnet_mask subnet, ip4_addr ip);

