#include "ptp_net_id.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "lib/var_len_encoding.h"

bool ptp_net_id::operator==(const ptp_net_id &rhs) const {
    if (vals.size() != rhs.vals.size()) return false;

    for (int i = 0; i < vals.size(); i++)
        if (vals[i] != rhs.vals[i])
            return false;
    
    return true;
}

bool ptp_net_id::operator!=(const ptp_net_id &rhs) const {
    return !(rhs == *this);
}


optional<ptp_net_id> str_to_ptp_net_id(string const& str) {
    int n = str.size();

    ptp_net_id addr;

    int i = 0;
    while (i < str.size()) {
        if (i != 0) {
            if (str[i] != '.') return {};
            ++i;
        }
        
        if (!(n - i >= 2)) return {};

        try {
            uint8_t octet = std::stoul(str.substr(i, 2), nullptr, 16);
            addr.vals.push_back(octet);
        } catch (...) {
            return {};
        }
    }

    return addr;
}

string ptp_net_id_to_str(ptp_net_id const& ptp_net_id) {
    string res;

    for (int i = 0; i < ptp_net_id.vals.size(); i++) {
        if (i > 0) res += '.';

        std::stringstream ss;
        ss << std::hex << static_cast<int>(ptp_net_id.vals[i]);
        res += ss.str();
    }

    return res;
}

int write_in_network_order(uint8_t* dest, ptp_net_id const* src) {
    return var_len_encode_bytes(dest, src->vals);
}

int extract_from_network_order(ptp_net_id* dest, uint8_t const* src) {
    return var_len_decode_bytes(&dest->vals, src);
}
