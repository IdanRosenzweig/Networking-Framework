#pragma once

#include <cstdint>
#include <string>
#include <optional>
using namespace std;

enum ethertype : uint16_t {
    ip4 = 0x0800,
    arp = 0x0806,
};

optional<string> ethertype_to_str(ethertype prot);

optional<ethertype> str_to_ethertype(string const& str);