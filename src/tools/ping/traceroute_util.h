#ifndef NETWORKING_TRACEROUTE_UTIL_H
#define NETWORKING_TRACEROUTE_UTIL_H

#include "../../abstract/gateway/gateway.h"
#include "../../abstract/receiving/recv_queue.h"
#include "../../protocol_stacks/icmp_stack.h"

class traceroute_util {
    gateway * network_layer_gw;
    icmp_protocol_stack prot_stack;
    recv_queue<recv_msg_t> replies_recv_q;

public:
    traceroute_util(ip4_addr src_ip, gateway* gw);

    ~traceroute_util();

    next_choice<ip4_addr> dest_ip;

    void trace_to_destination();

};


#endif //NETWORKING_TRACEROUTE_UTIL_H
