#pragma once

#include <string>
#include <optional>
using namespace std;

#include "eap_addr.h"

enum eap_prot_values : uint8_t {
    icmp = 1,
    tcp = 6,
    udp = 17
};
optional<string> eap_prot_to_str(eap_prot_values prot);
optional<eap_prot_values> str_to_eap_prot(string const& str);

struct eap_header {
    eap_addr src_addr;
    eap_addr dest_addr;

    uint8_t no_jumps;

    eap_prot_values prot;
};

int write_in_network_order(uint8_t* dest, eap_header const* src);

int extract_from_network_order(eap_header* dest, uint8_t const* src);
