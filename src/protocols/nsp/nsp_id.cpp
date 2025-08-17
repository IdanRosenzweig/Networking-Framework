#include "nsp_id.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "lib/var_len_encoding.h"

bool nsp_id::operator==(const nsp_id &rhs) const {
    if (vals.size() != rhs.vals.size()) return false;

    for (int i = 0; i < vals.size(); i++)
        if (vals[i] != rhs.vals[i])
            return false;
    
    return true;
}

bool nsp_id::operator!=(const nsp_id &rhs) const {
    return !(rhs == *this);
}


optional<nsp_id> str_to_nsp_id(string const& str) {
    int n = str.size();

    nsp_id addr;

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

string nsp_id_to_str(nsp_id const& nsp_id) {
    string res;

    for (int i = 0; i < nsp_id.vals.size(); i++) {
        if (i > 0) res += '.';

        std::stringstream ss;
        ss << std::hex << static_cast<int>(nsp_id.vals[i]);
        res += ss.str();
    }

    return res;
}

int write_in_network_order(uint8_t* dest, nsp_id const* src) {
    return var_len_encode_bytes(dest, src->vals);
}

int extract_from_network_order(nsp_id* dest, uint8_t const* src) {
    return var_len_decode_bytes(&dest->vals, src);
}
