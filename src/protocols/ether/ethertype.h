#pragma once

#include <cstdint>

enum ethertype : uint16_t {
    ip4 = 0x0800,
    arp = 0x0806,
};