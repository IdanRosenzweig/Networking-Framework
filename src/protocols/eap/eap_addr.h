#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
using namespace std;

struct eap_addr {
    vector<uint8_t> octets;

    bool operator==(const eap_addr &rhs) const;

    bool operator!=(const eap_addr &rhs) const;

    bool operator<(const eap_addr &rhs) const;

    bool operator>(const eap_addr &rhs) const;

    bool operator<=(const eap_addr &rhs) const;

    bool operator>=(const eap_addr &rhs) const;
};

extern eap_addr eap_addr_empty;
extern eap_addr eap_addr_broadcast;

optional<eap_addr> str_to_eap_addr(string const& str);

string eap_addr_to_str(eap_addr const& eap_addr);

int write_in_network_order(uint8_t* dest, eap_addr const* src);

int extract_from_network_order(eap_addr* dest, uint8_t const* src);


struct eap_subnet_mask {
    eap_addr prefix;
};

optional<eap_subnet_mask> str_to_eap_subnet_mask(string const& str);

string eap_subnet_mask_to_str(eap_subnet_mask const& subnet);

eap_addr smallest_ip_addr_in_subnet(eap_subnet_mask const& subnet);

bool is_inside_subnet(eap_subnet_mask const& subnet, eap_addr const& ip);

