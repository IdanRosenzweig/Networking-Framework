#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
using namespace std;

struct ptp_net_id {
    vector<uint8_t> vals;

    bool operator==(const ptp_net_id &rhs) const;

    bool operator!=(const ptp_net_id &rhs) const;

};

optional<ptp_net_id> str_to_ptp_net_id(string const& str);

string ptp_net_id_to_str(ptp_net_id const& ptp_net_id);

int write_in_network_order(uint8_t* dest, ptp_net_id const* src);

int extract_from_network_order(ptp_net_id* dest, uint8_t const* src);

