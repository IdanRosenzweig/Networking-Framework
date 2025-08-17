#include "ptp_endpoint.h"

#include "lib/var_len_encoding.h"

int write_in_network_order(uint8_t* dest, ptp_endpoint const* src) {
    uint8_t* curr = dest;

    *curr++ = static_cast<uint8_t>(src->type);

    return curr - dest;
}

int extract_from_network_order(ptp_endpoint* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    dest->type = static_cast<ptp_endpoint_t>(*curr++);

    return curr - src;
}

optional<string> ptp_endpoint_to_str(ptp_endpoint const& endpoint) {
    switch (endpoint.type) {
        case ptp_endpoint_t::start: return "start";
        case ptp_endpoint_t::end: return "end";
        default: return {};
    }
}

optional<ptp_endpoint> str_to_ptp_endpoint(string const& str) {
    if (str == "start") return ptp_endpoint{ptp_endpoint_t::start};
    else if (str == "end") return ptp_endpoint{ptp_endpoint_t::end};
    else return {};
}