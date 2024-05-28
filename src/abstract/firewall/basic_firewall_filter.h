#ifndef NETWORKING_BASIC_FIREWALL_FILTER_H
#define NETWORKING_BASIC_FIREWALL_FILTER_H

#include "firewall_policy.h"
#include "../receiving/msg/received_msg.h"

template <typename TYPE>
class basic_firewall_filter {
public:
    // produce policy decision about the value
    virtual firewall_policy calc_policy(const TYPE& val) = 0;

};
#endif //NETWORKING_BASIC_FIREWALL_FILTER_H
