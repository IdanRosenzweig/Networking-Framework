#ifndef NETWORKING_PING_UTIL_H
#define NETWORKING_PING_UTIL_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../abstract/utils/circular_buffer.h"
#include "../../abstract/gateway/gateway.h"

class ping_util : private recv_queue<received_msg> {
    gateway *network_layer_gw;
    ip4_protocol ip_prot;
    icmp_protocol icmp_prot;

public:
    ping_util(ip4_addr src_ip, gateway* gw);

    ~ping_util();

    next_choice<ip4_addr> dest_ip;
    next_choice<int> count;
    next_choice<std::chrono::duration<uint64_t, milli>> delay_interval;

    void ping_node();

};


#endif //NETWORKING_PING_UTIL_H
