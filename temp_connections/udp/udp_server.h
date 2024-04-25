#ifndef SERVERCLIENT_UDP_SERVER_H
#define SERVERCLIENT_UDP_SERVER_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../linux/network_layer_gateway/network_layer_gateway.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../abstract/basic_connection.h"
#include "../../protocols/udp/udp_header.h"
#include <linux/ip.h>

struct socket_data {
    ip4_addr client_addr;
    int client_source_port;
    int dest_port;
    received_msg data;
};

class udp_server : public msg_receiver, public listenable<socket_data> {
public:
    network_layer_gateway gateway;
    ip4_protocol ip_server;
    udp_protocol _udp_server;

    int server_port;

    udp_server(int serverPort) : server_port(serverPort) {

        // setup send to client flow
        ip_server.gateway = &gateway;
        ip_server.next_protocol.set_next_choice(IPPROTO_UDP);

        _udp_server.gateway = &ip_server;
        _udp_server.next_source_port.set_next_choice(server_port);


        // setup recv from client flow
        gateway.add_listener(&ip_server);

        ip_server.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_server);

        _udp_server.port_handlers.assign_to_key(server_port, this);
    }

    void handle_received_event(received_msg event) override {
        socket_data data;
        data.data = event;
        data.client_source_port =
                ntohs( ((struct udp_header*) (event.data.get() + event.protocol_offsets.rbegin()->first))->source_port );
        data.dest_port =
                ntohs( ((struct udp_header*) (event.data.get() + event.protocol_offsets.rbegin()->first))->dest_port );
        data.client_addr = ip4_addr{
                ntohl(((struct iphdr*) (event.data.get() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr)        };

        this->listenable::handle_received_event(data);
    }

    int send_data_to_client(const string& client_ip, int dest_port, send_msg msg) {
        ip_server.next_dest_addr.set_next_choice(convert_to_ip4_addr(client_ip));
        _udp_server.next_dest_port.set_next_choice(dest_port);

        return _udp_server.send_data(msg);
    }

};

#endif //SERVERCLIENT_UDP_SERVER_H
