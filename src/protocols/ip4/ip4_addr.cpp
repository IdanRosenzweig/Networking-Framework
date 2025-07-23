#include "ip4_addr.h"
#include <iostream>
#include <cstring>

bool ip4_addr::operator==(const ip4_addr &rhs) const {
    for (int i = 0; i < sizeof(octets); i++)
        if (octets[i] != rhs.octets[i])
            return false;
    return true;
}

bool ip4_addr::operator!=(const ip4_addr &rhs) const {
    return !(rhs == *this);
}

bool ip4_addr::operator<(const ip4_addr &rhs) const {
    for (int i = 0; i < sizeof(octets); i++)
        if (octets[i] != rhs.octets[i])
            return octets[i] < rhs.octets[i];
    return false;
}

bool ip4_addr::operator>(const ip4_addr &rhs) const {
    return rhs < *this;
}

bool ip4_addr::operator<=(const ip4_addr &rhs) const {
    return !(rhs < *this);
}

bool ip4_addr::operator>=(const ip4_addr &rhs) const {
    return !(*this < rhs);
}

ip4_addr ip4_addr_empty = ip4_addr{{0,0,0,0}};

ip4_addr generate_next_ip(ip4_addr addr) {
    ip4_addr next_ip;

    int curr_in = sizeof(ip4_addr::octets) - 1;
    while (curr_in >= 0 && (addr.octets[curr_in] == (uint8_t) ~((uint8_t) 0))) {
        next_ip.octets[curr_in] = 0;
        curr_in--;
    }
    if (curr_in >= 0) next_ip.octets[curr_in] = addr.octets[curr_in] + 1;
    for (int i = curr_in - 1; i >= 0; i--)
        next_ip.octets[i] = addr.octets[i];

    return next_ip;
}

optional<ip4_addr> str_to_ip4_addr(string const& str) {
    unsigned int a, b, c, d;
    if (sscanf(str.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) != 4) return {};

    ip4_addr addr;
    addr.octets[0] = a;
    addr.octets[1] = b;
    addr.octets[2] = c;
    addr.octets[3] = d;

    return addr;
}

string ip4_addr_to_str(ip4_addr const& ip_addr) {
    return to_string(ip_addr.octets[0])
           + "." + to_string(ip_addr.octets[1])
           + "." + to_string(ip_addr.octets[2])
           + "." + to_string(ip_addr.octets[3]);
}

int write_in_network_order(uint8_t* dest, ip4_addr const* source) {
    memcpy(dest, source->octets, sizeof(ip4_addr::octets));
    return sizeof(ip4_addr::octets);
}

int extract_from_network_order(ip4_addr* dest, uint8_t const* source) {
    memcpy(dest->octets, source, sizeof(ip4_addr::octets));
    return sizeof(ip4_addr::octets);
}


optional<ip4_subnet_mask> str_to_ip4_subnet_mask(string const& str) {
    auto slash = str.find('/');
    if (slash == string::npos) return {};

    ip4_addr ip;
    if (auto res = str_to_ip4_addr(str.substr(0, slash)); res.has_value()) ip = res.value();
    else return {};

    int mask = std::stoi(str.substr(slash + 1));
    if (!(0 <= mask <= 32)) return {};

    return {{ip, mask}};
}

string ip4_subnet_mask_to_str(ip4_subnet_mask mask) {
    return ip4_addr_to_str(mask.base_addr) + "/" + to_string(mask.no_mask_bits);
}

ip4_addr smallest_ip_addr_in_subnet(ip4_subnet_mask subnet) {
    int bits = subnet.no_mask_bits;
    int index = bits / 8;
    bits %= 8;

    ip4_addr base_addr = subnet.base_addr;
    base_addr.octets[index] &= ((1 << bits) - 1); // keep only the reminder bits from the number
    for (int i = index + 1; i < sizeof(ip4_addr::octets); i++)
        base_addr.octets[i] = 0;

    return base_addr;
}

bool is_inside_subnet(ip4_subnet_mask subnet, ip4_addr ip) {
    int bits = subnet.no_mask_bits;
    int index = bits / 8;
    bits %= 8;

    bool inside_mask = true;
    for (int i = 0; i < index; i++)
        if (subnet.base_addr.octets[i] != ip.octets[i]) {
            inside_mask = false;
            break;
        }
    if ((subnet.base_addr.octets[index] & ((1 << bits) - 1)) !=
        (ip.octets[index] & ((1 << bits) - 1)))
        inside_mask = false;

    return inside_mask;
}

