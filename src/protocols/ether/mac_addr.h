#ifndef NETWORKING_MAC_ADDR_H
#define NETWORKING_MAC_ADDR_H

#include <stdint.h>
#include <string>

struct mac_addr {
    uint8_t octets[6];

    bool operator==(const mac_addr &rhs) const {
        return octets[0] == rhs.octets[0]
               && octets[1] == rhs.octets[1]
               && octets[2] == rhs.octets[2]
               && octets[3] == rhs.octets[3]
               && octets[4] == rhs.octets[4]
               && octets[5] == rhs.octets[5];
    }

    bool operator!=(const mac_addr &rhs) const {
        return !(rhs == *this);
    }
};

#define BROADCAST_MAC (mac_addr{0xff,0xff,0xff,0xff,0xff,0xff})

mac_addr convert_to_mac_addr(const std::string& str);

std::string convert_to_str(mac_addr addr);

#endif //NETWORKING_MAC_ADDR_H
