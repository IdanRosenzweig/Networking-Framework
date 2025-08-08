#include "emp_port.h"

#include <iostream>
#include <cstring>
#include <sstream>

#include "lib/var_len_encoding.h"

bool emp_port::operator==(const emp_port &rhs) const {
    if (vals.size() != rhs.vals.size()) return false;

    for (int i = 0; i < vals.size(); i++)
        if (vals[i] != rhs.vals[i])
            return false;
    
    return true;
}

bool emp_port::operator!=(const emp_port &rhs) const {
    return !(rhs == *this);
}

emp_port emp_port_empty = emp_port{{0}};

optional<emp_port> str_to_emp_port(string const& str) {
    int n = str.size();

    emp_port addr;

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

string emp_port_to_str(emp_port const& emp_port) {
    string res;

    for (int i = 0; i < emp_port.vals.size(); i++) {
        if (i > 0) res += '.';

        std::stringstream ss;
        ss << std::hex << static_cast<int>(emp_port.vals[i]);
        res += ss.str();
    }

    return res;
}

int write_in_network_order(uint8_t* dest, emp_port const* src) {
    return var_len_encode_bytes(dest, src->vals);
}

int extract_from_network_order(emp_port* dest, uint8_t const* src) {
    return var_len_decode_bytes(&dest->vals, src);
}
