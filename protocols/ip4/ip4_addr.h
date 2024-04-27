#ifndef SERVERCLIENT_IP4_ADDR_H
#define SERVERCLIENT_IP4_ADDR_H

#include <stdint.h>

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

#include <string>
using namespace std;

ip4_addr convert_to_ip4_addr(const string &str);

void write_in_network_order(uint8_t* dest, ip4_addr* source);

void extract_from_network_order(ip4_addr* dest, uint8_t* source);

#endif //SERVERCLIENT_IP4_ADDR_H
