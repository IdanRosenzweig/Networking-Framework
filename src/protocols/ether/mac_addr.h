#pragma once

#include <cstdint>
#include <string>
#include <optional>
using namespace std;

struct mac_addr {
    uint8_t octets[6];

    bool operator==(const mac_addr &rhs) const;

    bool operator!=(const mac_addr &rhs) const;

    bool operator<(const mac_addr &rhs) const;

    bool operator>(const mac_addr &rhs) const;

    bool operator<=(const mac_addr &rhs) const;

    bool operator>=(const mac_addr &rhs) const;
};

extern mac_addr mac_addr_broadcast;
extern mac_addr mac_addr_empty;

optional<mac_addr> str_to_mac_addr(string const& str);
// mac_addr convert_to_mac_addr(const std::string& str);

// std::string convert_to_str(mac_addr addr);
string mac_addr_to_str(mac_addr const& addr);

int write_in_network_order(uint8_t* dest, mac_addr const* source);

int extract_from_network_order(mac_addr* dest, uint8_t const* source);
