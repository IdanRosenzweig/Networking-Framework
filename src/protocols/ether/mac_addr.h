#ifndef SERVERCLIENT_MAC_ADDR_H
#define SERVERCLIENT_MAC_ADDR_H

#include <stdint.h>

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

void print_mac(mac_addr addr);

#endif //SERVERCLIENT_MAC_ADDR_H
