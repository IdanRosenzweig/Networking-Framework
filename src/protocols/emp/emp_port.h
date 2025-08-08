#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
using namespace std;

struct emp_port {
    vector<uint8_t> vals;

    bool operator==(const emp_port &rhs) const;

    bool operator!=(const emp_port &rhs) const;

};

extern emp_port emp_port_empty;

optional<emp_port> str_to_emp_port(string const& str);

string emp_port_to_str(emp_port const& emp_port);

int write_in_network_order(uint8_t* dest, emp_port const* src);

int extract_from_network_order(emp_port* dest, uint8_t const* src);

