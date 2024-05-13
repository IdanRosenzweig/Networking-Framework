#include "ip4_addr.h"
#include <arpa/inet.h>

bool ip4_addr::operator==(const ip4_addr &rhs) const {
    return octets[0] == rhs.octets[0]
           && octets[1] == rhs.octets[1]
           && octets[2] == rhs.octets[2]
           && octets[3] == rhs.octets[3];
}

bool ip4_addr::operator!=(const ip4_addr &rhs) const {
    return !(rhs == *this);
}

bool ip4_addr::operator<(const ip4_addr &rhs) const {
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

bool ip4_addr::operator>(const ip4_addr &rhs) const {
    return rhs < *this;
}

bool ip4_addr::operator<=(const ip4_addr &rhs) const {
    return !(rhs < *this);
}

bool ip4_addr::operator>=(const ip4_addr &rhs) const {
    return !(*this < rhs);
}


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

ip4_addr convert_to_ip4_addr(const string &str) {
    int a, b, c, d;
    if (sscanf(str.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        throw "can't convert ip4 address";
    }

    ip4_addr addr;
    addr.octets[0] = a;
    addr.octets[1] = b;
    addr.octets[2] = c;
    addr.octets[3] = d;

    return addr;
}

string convert_to_str(ip4_addr ip_addr) {
    return to_string(ip_addr.octets[0])
           + "." + to_string(ip_addr.octets[1])
           + "." + to_string(ip_addr.octets[2])
           + "." + to_string(ip_addr.octets[3]);
}

void write_in_network_order(uint8_t *dest, ip4_addr *source) {
    dest[0] = source->octets[0];
    dest[1] = source->octets[1];
    dest[2] = source->octets[2];
    dest[3] = source->octets[3];
}

void extract_from_network_order(ip4_addr *dest, uint8_t *source) {
    dest->octets[0] = source[0];
    dest->octets[1] = source[1];
    dest->octets[2] = source[2];
    dest->octets[3] = source[3];
}


ip4_subnet_mask convert_to_ip4_subnet_mask(const string &str) {
    int slash = str.find('/');
    return {convert_to_ip4_addr(str.substr(0, slash)),
            std::stoi(str.substr(slash + 1))};
}

string convert_to_str(ip4_subnet_mask mask) {
    return convert_to_str(mask.base_addr) + "/" + to_string(mask.no_mask_bits);
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

