#ifndef SERVERCLIENT_UDP_SERVER_H
#define SERVERCLIENT_UDP_SERVER_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"

#include "../../protocols/socket/socket_msg.h"
#include "../../protocols/udp/udp_header.h"

struct udp_packet_stack {
    received_msg msg;
    ip4_addr source_addr;
    int source_port;
    int dest_port;

    udp_packet_stack() {}
    udp_packet_stack(const udp_packet_stack& other) {
        operator=(other);
    }

    udp_packet_stack& operator=(const udp_packet_stack& other) {
        msg = other.msg;
        source_addr = other.source_addr;
        source_port = other.source_port;
        dest_port = other.dest_port;
        return *this;
    }
};

class udp_server : public msg_receiver, public receive_forwarder<udp_packet_stack> {
public:

    gateway *network_layer_gw;
    ip4_protocol ip_server;
    udp_protocol _udp_server;

    int server_port;

    udp_server(int serverPort, ip4_addr src_ip, gateway* gw);

    ~udp_server();

    int send_data_to_client(ip4_addr dest_addr, int dest_port, send_msg<> msg);

    void handle_received_event(received_msg &&event) override;

};

#endif //SERVERCLIENT_UDP_SERVER_H
