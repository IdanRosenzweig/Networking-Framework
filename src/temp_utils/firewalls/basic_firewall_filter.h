#ifndef SERVERCLIENT_BASIC_FIREWALL_FILTER_H
#define SERVERCLIENT_BASIC_FIREWALL_FILTER_H

#include "firewall_policy.h"
#include "../../abstract/receiving/received_msg.h"

class basic_firewall_filter {
public:
    // produce policy decision about the packet
    virtual firewall_policy calc_policy(received_msg& msg) = 0;

};
#endif //SERVERCLIENT_BASIC_FIREWALL_FILTER_H
