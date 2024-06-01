#ifndef NETWORKING_MAC_ADDR_H
#define NETWORKING_MAC_ADDR_H

#include <stdint.h>
#include <string>

struct mac_addr {
    uint8_t octets[6];

    bool operator==(const mac_addr &rhs) const;

    bool operator!=(const mac_addr &rhs) const;

    bool operator<(const mac_addr &rhs) const;

    bool operator>(const mac_addr &rhs) const;

    bool operator<=(const mac_addr &rhs) const;

    bool operator>=(const mac_addr &rhs) const;
};

#define BROADCAST_MAC (mac_addr{0xff,0xff,0xff,0xff,0xff,0xff})

mac_addr convert_to_mac_addr(const std::string& str);

std::string convert_to_str(mac_addr addr);

int write_in_network_order(uint8_t* dest, mac_addr* source);

int extract_from_network_order(mac_addr* dest, uint8_t* source);

#endif //NETWORKING_MAC_ADDR_H
