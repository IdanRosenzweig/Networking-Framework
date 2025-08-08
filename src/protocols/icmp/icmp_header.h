#pragma once

#include <cstdint>

enum class icmp_type : uint8_t {
    echoreply = 0,
    dest_unreachable = 3,
    redirect = 5,
    echo = 8,
    time_exceeded = 11
};

struct icmp_header {
    icmp_type type;

    uint8_t code;
    uint16_t checksum;
    union content {
        uint32_t raw;
        struct echo_content {
            uint16_t id;
            uint16_t sequence;
        } echo; // content for type echo
        struct redirect_content {
            uint32_t gateway; // content for type redirect
        } redirect;
        struct dest_unreachable_content {
            uint16_t _unused;
            uint16_t mtu;
        } dest_unreach; // content for type destination unreachable
    } content; // content varying for different types

};

int write_in_network_order(uint8_t* dest, icmp_header const* src);

int extract_from_network_order(icmp_header* dest, uint8_t const* src);

