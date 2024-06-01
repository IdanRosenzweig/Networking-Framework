#ifndef NETWORKING_UDP_CLIENT_H
#define NETWORKING_UDP_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/udp_stack.h"

class udp_client : public connection, public recv_forwarder<udp_packet_stack> {
public:
    gateway *network_layer_gw;
    udp_protocol_stack prot_stack;

    udp_client(ip4_addr dest_ip, uint16_t dest_port, uint16_t my_port, ip4_addr src_ip, gateway *gw);

    ~udp_client();

    int send_data(send_msg_t &&data) override;

    void handle_callback(recv_msg_t &&data) override;

};


#endif //NETWORKING_UDP_CLIENT_H
