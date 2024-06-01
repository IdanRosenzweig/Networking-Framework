#ifndef NETWORKING_PING_UTIL_H
#define NETWORKING_PING_UTIL_H

#include "../../abstract/gateway/gateway.h"
#include "../../abstract/receiving/recv_queue.h"
#include "../../protocol_stacks/icmp_stack.h"

class ping_util {
    gateway *network_layer_gw;
    icmp_protocol_stack prot_stack;
    recv_queue<recv_msg_t> replies_recv_q;

public:
    ping_util(ip4_addr src_ip, gateway* gw);

    ~ping_util();

    next_choice<ip4_addr> dest_ip;
    next_choice<int> count;
    next_choice<std::chrono::duration<uint64_t, milli>> delay_interval;

    void ping_node();

};


#endif //NETWORKING_PING_UTIL_H
