#pragma once

#include <cstdint>

enum class arp_op_values : uint16_t {
    request = 1,
    reply = 2,
};