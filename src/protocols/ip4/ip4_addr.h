#ifndef SERVERCLIENT_IP4_ADDR_H
#define SERVERCLIENT_IP4_ADDR_H

#include <stdint.h>
#include <string>
using namespace std;

struct ip4_addr {
    uint8_t octets[4];

    bool operator==(const ip4_addr &rhs) const {
        return octets[0] == rhs.octets[0]
               && octets[1] == rhs.octets[1]
               && octets[2] == rhs.octets[2]
               && octets[3] == rhs.octets[3];
    }

    bool operator!=(const ip4_addr &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const ip4_addr &rhs) const {
        if (octets[0] != rhs.octets[0])
            return octets[0] < rhs.octets[0];

        if (octets[1] != rhs.octets[1])
            return octets[1] < rhs.octets[1];

        if (octets[2] != rhs.octets[2])
            return octets[2] < rhs.octets[2];

        if (octets[3] != rhs.octets[3])
            return octets[3] < rhs.octets[3];

        return false;
    }

    bool operator>(const ip4_addr &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const ip4_addr &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const ip4_addr &rhs) const {
        return !(*this < rhs);
    }
};

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

#endif //SERVERCLIENT_IP4_ADDR_H
