#include "mac_addr.h"
#include <stdio.h>
#include <cstring>

bool mac_addr::operator==(const mac_addr &rhs) const {
    for (int i = 0; i < sizeof(octets); i++)
        if (octets[i] != rhs.octets[i])
            return false;
    return true;
}

bool mac_addr::operator!=(const mac_addr &rhs) const {
    return !(rhs == *this);
}

bool mac_addr::operator<(const mac_addr &rhs) const {
    for (int i = 0; i < sizeof(octets); i++)
        if (octets[i] != rhs.octets[i])
            return octets[i] < rhs.octets[i];
    return false;
}

bool mac_addr::operator>(const mac_addr &rhs) const {
    return rhs < *this;
}

bool mac_addr::operator<=(const mac_addr &rhs) const {
    return !(rhs < *this);
}

bool mac_addr::operator>=(const mac_addr &rhs) const {
    return !(*this < rhs);
}

mac_addr convert_to_mac_addr(const std::string &str) {
    mac_addr addr;
    sscanf(str.c_str(), "%x:%x:%x:%x:%x:%x",
           (unsigned int *) &addr.octets[0],
           (unsigned int *) &addr.octets[1],
           (unsigned int *) &addr.octets[2],
           (unsigned int *) &addr.octets[3],
           (unsigned int *) &addr.octets[4],
           (unsigned int *) &addr.octets[5]
    );

    return addr;
}

std::string convert_to_str(mac_addr addr) {
    char buff[2 * 6 + 5 + 1] = {0};
    sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned int) addr.octets[0],
            (unsigned int) addr.octets[1],
            (unsigned int) addr.octets[2],
            (unsigned int) addr.octets[3],
            (unsigned int) addr.octets[4],
            (unsigned int) addr.octets[5]);

    return std::string(buff);
}

int write_in_network_order(uint8_t* dest, mac_addr* source) {
    memcpy(dest, source->octets, sizeof(mac_addr::octets));
    return sizeof(mac_addr::octets);
}

int extract_from_network_order(mac_addr* dest, uint8_t* source) {
    memcpy(dest->octets, source, sizeof(mac_addr::octets));
    return sizeof(mac_addr::octets);
}