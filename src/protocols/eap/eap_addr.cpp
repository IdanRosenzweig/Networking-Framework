#include "eap_addr.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "lib/var_len_encoding.h"

bool eap_addr::operator==(const eap_addr &rhs) const {
    if (octets.size() != rhs.octets.size()) return false;

    for (int i = 0; i < octets.size(); i++)
        if (octets[i] != rhs.octets[i])
            return false;
    
    return true;
}

bool eap_addr::operator!=(const eap_addr &rhs) const {
    return !(rhs == *this);
}

bool eap_addr::operator<(const eap_addr &rhs) const {
    for (int i = 0; i < octets.size(); i++) {
        if (!(i < rhs.octets.size())) return false;

        if (octets[i] != rhs.octets[i])
            return octets[i] < rhs.octets[i];
    }

    return octets.size() < rhs.octets.size();
}

bool eap_addr::operator>(const eap_addr &rhs) const {
    return rhs < *this;
}

bool eap_addr::operator<=(const eap_addr &rhs) const {
    return !(rhs < *this);
}

bool eap_addr::operator>=(const eap_addr &rhs) const {
    return !(*this < rhs);
}

eap_addr eap_addr_empty = eap_addr{{0}};
eap_addr eap_addr_broadcast = eap_addr{{0xff}};

optional<eap_addr> str_to_eap_addr(string const& str) {
    int n = str.size();

    eap_addr addr;

    int i = 0;
    while (i < str.size()) {
        if (i != 0) {
            if (str[i] != '.') return {};
            ++i;
        }
        
        if (!(n - i >= 2)) return {};

        try {
            uint8_t octet = std::stoul(str.substr(i, 2), nullptr, 16);
            addr.octets.push_back(octet);
        } catch (...) {
            return {};
        }
    }

    return addr;
}

string eap_addr_to_str(eap_addr const& eap_addr) {
    string res;

    for (int i = 0; i < eap_addr.octets.size(); i++) {
        if (i > 0) res += '.';

        std::stringstream ss;
        ss << std::hex << static_cast<int>(eap_addr.octets[i]);
        res += ss.str();
    }

    return res;
}

int write_in_network_order(uint8_t* dest, eap_addr const* src) {
    return var_len_encode_bytes(dest, src->octets);
}

int extract_from_network_order(eap_addr* dest, uint8_t const* src) {
    return var_len_decode_bytes(&dest->octets, src);
}


optional<eap_subnet_mask> str_to_eap_subnet_mask(string const& str) {
    if (str.back() != '/') return {};
    
    if (auto addr = str_to_eap_addr(str.substr(0, str.size() - 1)); addr.has_value()) return eap_subnet_mask{addr.value()};
    else return {};
}

string eap_subnet_mask_to_str(eap_subnet_mask const& subnet) {
    return eap_addr_to_str(subnet.prefix) + "/";
}

eap_addr smallest_ip_addr_in_subnet(eap_subnet_mask const& subnet) {
    eap_addr addr = subnet.prefix;
    addr.octets.push_back(0);

    return addr;
}

bool is_inside_subnet(eap_subnet_mask const& subnet, eap_addr const& eap_addr) {
    if (!(subnet.prefix.octets.size() < eap_addr.octets.size())) return false;

    for (int i = 0; i < subnet.prefix.octets.size(); i++)
        if (subnet.prefix.octets[i] != eap_addr.octets[i])
            return false;

    return true;
}
