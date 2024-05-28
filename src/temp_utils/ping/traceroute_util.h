#ifndef NETWORKING_TRACEROUTE_UTIL_H
#define NETWORKING_TRACEROUTE_UTIL_H

#include "ping_util.h"

class traceroute_util : private recv_queue<received_msg> {
    gateway * network_layer_gw;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

public:
    traceroute_util(ip4_addr src_ip, gateway* gw);

    ~traceroute_util();

    next_choice<ip4_addr> dest_ip;

    void trace_to_destination();

};


#endif //NETWORKING_TRACEROUTE_UTIL_H
