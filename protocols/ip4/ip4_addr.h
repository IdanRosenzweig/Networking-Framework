#ifndef SERVERCLIENT_IP4_ADDR_H
#define SERVERCLIENT_IP4_ADDR_H

#include <stdint.h>

struct ip4_addr {
    uint32_t raw;

    bool operator==(const ip4_addr &rhs) const {
        return raw == rhs.raw;
    }

    bool operator!=(const ip4_addr &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const ip4_addr &rhs) const {
        return raw < rhs.raw;
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

ip4_addr convert_to_ip4_addr(const string& str);

#endif //SERVERCLIENT_IP4_ADDR_H
