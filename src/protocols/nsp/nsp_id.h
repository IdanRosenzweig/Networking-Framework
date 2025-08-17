#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
using namespace std;

struct nsp_id {
    vector<uint8_t> vals;

    bool operator==(const nsp_id &rhs) const;

    bool operator!=(const nsp_id &rhs) const;

};

optional<nsp_id> str_to_nsp_id(string const& str);

string nsp_id_to_str(nsp_id const& nsp_id);

int write_in_network_order(uint8_t* dest, nsp_id const* src);

int extract_from_network_order(nsp_id* dest, uint8_t const* src);

