#ifndef NETWORKING_UDP_SERVER_H
#define NETWORKING_UDP_SERVER_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/udp_stack.h"

class udp_server : public msg_recv_listener, public recv_forwarder<udp_packet_stack> {
public:

    gateway *network_layer_gw;
    udp_protocol_stack prot_stack;

    uint16_t server_port;

    udp_server(uint16_t serverPort, ip4_addr src_ip, gateway* gw);

    ~udp_server();

    int send_data_to_client(ip4_addr dest_addr, uint16_t dest_port, send_msg_t&& msg);

    void handle_callback(recv_msg_t &&data) override;

};

#endif //NETWORKING_UDP_SERVER_H
