#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
using namespace std;

struct ptp_addr {
    vector<uint8_t> octets;

    bool operator==(const ptp_addr &rhs) const;

    bool operator!=(const ptp_addr &rhs) const;

    bool operator<(const ptp_addr &rhs) const;

    bool operator>(const ptp_addr &rhs) const;

    bool operator<=(const ptp_addr &rhs) const;

    bool operator>=(const ptp_addr &rhs) const;
};

extern ptp_addr ptp_addr_empty;
extern ptp_addr ptp_addr_broadcast;

optional<ptp_addr> str_to_ptp_addr(string const& str);

string ptp_addr_to_str(ptp_addr const& ptp_addr);

int write_in_network_order(uint8_t* dest, ptp_addr const* src);

int extract_from_network_order(ptp_addr* dest, uint8_t const* src);


struct ptp_subnet_mask {
    ptp_addr prefix;
};

optional<ptp_subnet_mask> str_to_ptp_subnet_mask(string const& str);

string ptp_subnet_mask_to_str(ptp_subnet_mask const& subnet);

ptp_addr smallest_ip_addr_in_subnet(ptp_subnet_mask const& subnet);

bool is_inside_subnet(ptp_subnet_mask const& subnet, ptp_addr const& ip);

