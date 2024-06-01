#ifndef NETWORKING_ICMP_SERVER_H
#define NETWORKING_ICMP_SERVER_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/icmp_stack.h"
#include "common.h"

class icmp_server : public msg_recv_listener, public recv_forwarder<icmp_packet_stack> {
public:
    gateway *network_layer_gw;
    icmp_protocol_stack prot_stack;

    icmp_server(ip4_addr dest_ip, ip4_addr src_ip, gateway* gw);

    ~icmp_server();

    int send_data_to_client(ip4_addr dest_addr, uint8_t type, send_msg_t&& msg);

    void handle_callback(recv_msg_t &&data) override;

};

#endif //NETWORKING_ICMP_SERVER_H
