#pragma once

#include <cstdint>
#include <optional>
using namespace std;

enum class ptp_endpoint_t : uint8_t {
    start, end
};

struct ptp_endpoint {
    ptp_endpoint_t type;
};

int write_in_network_order(uint8_t* dest, ptp_endpoint const* src);

int extract_from_network_order(ptp_endpoint* dest, uint8_t const* src);

optional<string> ptp_endpoint_to_str(ptp_endpoint const& endpoint);

optional<ptp_endpoint> str_to_ptp_endpoint(string const& str);