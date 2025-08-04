#pragma once

#include <string>
#include <optional>
using namespace std;

#include "ip4_addr.h"

enum ip_prot_values : uint8_t {
    icmp = 1,
    tcp = 6,
    udp = 17
};
optional<string> ip_prot_to_str(ip_prot_values prot);
optional<ip_prot_values> str_to_ip_prot(string const& str);

struct __attribute__((packed)) ip4_header {
    uint8_t version : 4;
    uint8_t header_len : 4;
    uint8_t type_of_service;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_off : 13;
    uint8_t ttl;

    ip_prot_values prot;

    uint16_t checksum;
    ip4_addr src_addr;
    ip4_addr dest_addr;
};

int write_in_network_order(uint8_t* dest, ip4_header const* src);

int extract_from_network_order(ip4_header* dest, uint8_t const* src);
