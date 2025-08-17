#include "ptp_net_switch.h"

#include "lib/var_len_encoding.h"
#include "lib/split_str.h"

int write_in_network_order(uint8_t* dest, ptp_net_switch const* src) {
    uint8_t* curr = dest;

    curr += var_len_encode_bytes(curr, src->src_id.vals);
    curr += var_len_encode_bytes(curr, src->dest_id.vals);

    return curr - dest;
}

int extract_from_network_order(ptp_net_switch* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    curr += var_len_decode_bytes(&dest->src_id.vals, curr);
    curr += var_len_decode_bytes(&dest->dest_id.vals, curr);

    return curr - src;
}

string ptp_net_switch_to_str(ptp_net_switch const& net_switch) {
    string res;

    res += ptp_net_id_to_str(net_switch.src_id);
    res += " -> ";
    res += ptp_net_id_to_str(net_switch.dest_id);

    return res;
}

optional<ptp_net_switch> str_to_ptp_net_switch(string const& str) {
    ptp_net_switch net_switch;

    auto addrs = split_str(str, " -> ");
    if (addrs.size() != 2) return {};

    if (auto tmp = str_to_ptp_net_id(addrs[0]); tmp.has_value()) net_switch.src_id = tmp.value();
    else return {};

    if (auto tmp = str_to_ptp_net_id(addrs[1]); tmp.has_value()) net_switch.dest_id = tmp.value();
    else return {};
    
    return net_switch;
}