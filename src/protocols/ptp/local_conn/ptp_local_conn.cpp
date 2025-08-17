#include "ptp_local_conn.h"

#include "lib/var_len_encoding.h"
#include "lib/split_str.h"

#include <iostream>

int write_in_network_order(uint8_t* dest, ptp_local_conn const* src) {
    uint8_t* curr = dest;

    curr += var_len_encode_bytes(curr, src->src_addr.octets);
    curr += var_len_encode_bytes(curr, src->dest_addr.octets);

    return curr - dest;
}

int extract_from_network_order(ptp_local_conn* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    curr += var_len_decode_bytes(&dest->src_addr.octets, curr);
    curr += var_len_decode_bytes(&dest->dest_addr.octets, curr);

    return curr - src;
}

string ptp_local_conn_to_str(ptp_local_conn const& local_conn) {
    string res;

    res += ptp_addr_to_str(local_conn.src_addr);
    res += " -> ";
    res += ptp_addr_to_str(local_conn.dest_addr);

    return res;
}

optional<ptp_local_conn> str_to_ptp_local_conn(string const& str) {
    ptp_local_conn conn;

    auto addrs = split_str(str, " -> ");
    if (addrs.size() != 2) return {};

    if (auto tmp = str_to_ptp_addr(addrs[0]); tmp.has_value()) conn.src_addr = tmp.value();
    else return {};

    if (auto tmp = str_to_ptp_addr(addrs[1]); tmp.has_value()) conn.dest_addr = tmp.value();
    else return {};
    
    return conn;
}