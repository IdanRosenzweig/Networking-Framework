#include "eap_header.h"

#include <netinet/in.h>

#include "lib/var_len_encoding.h"

optional<string> eap_prot_to_str(eap_prot_values prot) {
    switch (prot) {
        case eap_prot_values::icmp: return "icmp";
        case eap_prot_values::udp: return "udp";
        case eap_prot_values::tcp: return "tcp";
        default: return {};
    }
}

optional<eap_prot_values> str_to_eap_prot(string const& str) {
    if (str == "icmp") return eap_prot_values::icmp;
    else if (str == "udp") return eap_prot_values::udp;
    else if (str == "tcp") return eap_prot_values::tcp;
    else return {};
}

int write_in_network_order(uint8_t* dest, eap_header const* src) {
    uint8_t* curr = dest;

    // src and dest addrs
    curr += write_in_network_order(curr, &src->src_addr);
    curr += write_in_network_order(curr, &src->dest_addr);

    // no jumps
    *curr++ = src->no_jumps;

    // prot
    *curr++ = static_cast<uint8_t>(src->prot);

    return curr - dest;
}

int extract_from_network_order(eap_header* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    // src and dest addrs
    curr += extract_from_network_order(&dest->src_addr, curr);
    curr += extract_from_network_order(&dest->dest_addr, curr);

    // no jumps
    dest->no_jumps = *curr++;

    // prot
    dest->prot = static_cast<eap_prot_values>(*curr++);

    return curr - src;
}