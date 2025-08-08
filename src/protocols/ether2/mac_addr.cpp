#include "mac_addr.h"

#include <stdio.h>
#include <cstring>
#include <regex>
#include <string>
#include <array>
#include <sstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

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

mac_addr mac_addr_broadcast = mac_addr{{0xff,0xff,0xff,0xff,0xff,0xff}};
mac_addr mac_addr_empty = mac_addr{{0x00,0x00,0x00,0x00,0x00,0x00}};

optional<mac_addr> str_to_mac_addr(string const& str) {
    mac_addr addr;

    std::regex reg("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");

    if (!std::regex_match(str, reg)) return {};

    std::stringstream ss(str);
    std::string byteStr;
    char delimiter;
    for (int i = 0; i < 6; ++i) {
        if (i < 5)
            std::getline(ss, byteStr, (str.find(':') != std::string::npos) ? ':' : '-');
        else
            ss >> byteStr;
        
        addr.octets[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
    }

    return addr;
}
// mac_addr convert_to_mac_addr(const std::string &str) {
//     mac_addr addr;
//     sscanf(str.c_str(), "%x:%x:%x:%x:%x:%x",
//            (unsigned int *) &addr.octets[0],
//            (unsigned int *) &addr.octets[1],
//            (unsigned int *) &addr.octets[2],
//            (unsigned int *) &addr.octets[3],
//            (unsigned int *) &addr.octets[4],
//            (unsigned int *) &addr.octets[5]
//     );

//     return addr;
// }

string mac_addr_to_str(mac_addr const& addr) {
    char buff[2 * 6 + 5 + 1] = {0};
    snprintf(buff, sizeof(buff), "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned int) addr.octets[0],
            (unsigned int) addr.octets[1],
            (unsigned int) addr.octets[2],
            (unsigned int) addr.octets[3],
            (unsigned int) addr.octets[4],
            (unsigned int) addr.octets[5]
        );

    return string(buff);
}

int write_in_network_order(uint8_t* dest, mac_addr const* source) {
    memcpy(dest, source->octets, sizeof(mac_addr::octets));
    return sizeof(mac_addr::octets);
}

int extract_from_network_order(mac_addr* dest, uint8_t const* source) {
    memcpy(dest->octets, source, sizeof(mac_addr::octets));
    return sizeof(mac_addr::octets);
}