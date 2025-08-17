#include "ptp_addr.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "lib/var_len_encoding.h"

bool ptp_addr::operator==(const ptp_addr &rhs) const {
    if (octets.size() != rhs.octets.size()) return false;

    for (int i = 0; i < octets.size(); i++)
        if (octets[i] != rhs.octets[i])
            return false;
    
    return true;
}

bool ptp_addr::operator!=(const ptp_addr &rhs) const {
    return !(rhs == *this);
}

bool ptp_addr::operator<(const ptp_addr &rhs) const {
    for (int i = 0; i < octets.size(); i++) {
        if (!(i < rhs.octets.size())) return false;

        if (octets[i] != rhs.octets[i])
            return octets[i] < rhs.octets[i];
    }

    return octets.size() < rhs.octets.size();
}

bool ptp_addr::operator>(const ptp_addr &rhs) const {
    return rhs < *this;
}

bool ptp_addr::operator<=(const ptp_addr &rhs) const {
    return !(rhs < *this);
}

bool ptp_addr::operator>=(const ptp_addr &rhs) const {
    return !(*this < rhs);
}

ptp_addr ptp_addr_empty = ptp_addr{{0}};
ptp_addr ptp_addr_broadcast = ptp_addr{{0xff}};

optional<ptp_addr> str_to_ptp_addr(string const& str) {
    int n = str.size();

    ptp_addr addr;

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
            i += 2;
            
        } catch (...) {
            return {};
        }
    }

    return addr;
}

string ptp_addr_to_str(ptp_addr const& ptp_addr) {
    string res;

    for (int i = 0; i < ptp_addr.octets.size(); i++) {
        if (i > 0) res += '.';

        std::stringstream ss;
        ss << std::hex << static_cast<int>(ptp_addr.octets[i]);
        res += ss.str();
    }

    return res;
}

int write_in_network_order(uint8_t* dest, ptp_addr const* src) {
    return var_len_encode_bytes(dest, src->octets);
}

int extract_from_network_order(ptp_addr* dest, uint8_t const* src) {
    return var_len_decode_bytes(&dest->octets, src);
}


optional<ptp_subnet_mask> str_to_ptp_subnet_mask(string const& str) {
    if (str.back() != '/') return {};
    
    if (auto addr = str_to_ptp_addr(str.substr(0, str.size() - 1)); addr.has_value()) return ptp_subnet_mask{addr.value()};
    else return {};
}

string ptp_subnet_mask_to_str(ptp_subnet_mask const& subnet) {
    return ptp_addr_to_str(subnet.prefix) + "/";
}

ptp_addr smallest_ip_addr_in_subnet(ptp_subnet_mask const& subnet) {
    ptp_addr addr = subnet.prefix;
    addr.octets.push_back(0);

    return addr;
}

bool is_inside_subnet(ptp_subnet_mask const& subnet, ptp_addr const& ptp_addr) {
    if (!(subnet.prefix.octets.size() < ptp_addr.octets.size())) return false;

    for (int i = 0; i < subnet.prefix.octets.size(); i++)
        if (subnet.prefix.octets[i] != ptp_addr.octets[i])
            return false;

    return true;
}
