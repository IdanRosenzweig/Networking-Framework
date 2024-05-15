#ifndef SERVERCLIENT_UDP_SERVER_H
#define SERVERCLIENT_UDP_SERVER_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../abstract/connection/msg_connection.h"

#include "../../protocols/socket/socket_msg.h"
#include "../../protocols/udp/udp_header.h"

class udp_server : private msg_receiver, public multi_receiver<socket_msg> {
    void handle_received_event(received_msg& event) override;

public:
    network_layer_gateway gateway;
    ip4_protocol ip_server;
    udp_protocol _udp_server;

    int server_port;

    udp_server(int serverPort);

    int send_data_to_client(ip4_addr client_addr, int dest_port, send_msg msg);

};

#endif //SERVERCLIENT_UDP_SERVER_H
