#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include <optional>
using namespace std;

#include "ptp_net_id.h"

struct ptp_net_switch {
    ptp_net_id src_id;
    ptp_net_id dest_id;
};

int write_in_network_order(uint8_t* dest, ptp_net_switch const* src);

int extract_from_network_order(ptp_net_switch* dest, uint8_t const* src);

string ptp_net_switch_to_str(ptp_net_switch const& net_switch);

optional<ptp_net_switch> str_to_ptp_net_switch(string const& str);