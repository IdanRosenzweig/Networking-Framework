#pragma once

#include "firewall_policy.h"

template <typename T>
class basic_firewall_filter {
public:
    // produce policy decision about the value
    virtual firewall_policy calc_policy(T const& val) = 0;

};
