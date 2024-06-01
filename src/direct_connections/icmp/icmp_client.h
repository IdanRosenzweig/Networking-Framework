#ifndef NETWORKING_ICMP_CLIENT_H
#define NETWORKING_ICMP_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/icmp_stack.h"
#include "common.h"

class icmp_client : public connection, public recv_forwarder<icmp_packet_stack> {
public:
    gateway * network_layer_gw;
    icmp_protocol_stack prot_stack;

    icmp_client(ip4_addr dest_ip, ip4_addr src_ip, gateway * gw);

    ~icmp_client();

    int send_data(send_msg_t&&data) override;

    void handle_callback(recv_msg_t &&data) override;

};

#endif //NETWORKING_ICMP_CLIENT_H
