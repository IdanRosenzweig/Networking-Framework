#pragma once

#include <cstdint>
#include <optional>
using namespace std;

#include "ptp_addr.h"

struct ptp_local_conn {
    ptp_addr src_addr;
    ptp_addr dest_addr;
};

int write_in_network_order(uint8_t* dest, ptp_local_conn const* src);

int extract_from_network_order(ptp_local_conn* dest, uint8_t const* src);

string ptp_local_conn_to_str(ptp_local_conn const& local_conn);

optional<ptp_local_conn> str_to_ptp_local_conn(string const& str);