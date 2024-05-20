#ifndef SERVERCLIENT_UDP_SERVER_H
#define SERVERCLIENT_UDP_SERVER_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../abstract/connection/msg_connection.h"

#include "../../protocols/socket/socket_msg.h"
#include "../../protocols/udp/udp_header.h"

struct udp_packet_stack {
    received_msg msg;
    ip4_addr source_addr;
    int source_port;
    int dest_port;
};

class udp_server : private msg_receiver, public multi_receiver<udp_packet_stack> {
    void handle_received_event(received_msg &event) override;

public:

    network_layer_gateway gateway;
    ip4_protocol ip_server;
    udp_protocol _udp_server;

    int server_port;

    udp_server(int serverPort);

    int send_data_to_client(ip4_addr dest_addr, int dest_port, send_msg<> msg);

};

#endif //SERVERCLIENT_BS_EMP_SERVER_H
