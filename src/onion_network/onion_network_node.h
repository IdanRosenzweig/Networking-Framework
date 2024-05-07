#ifndef SERVERCLIENT_ONION_NETWORK_NODE_H
#define SERVERCLIENT_ONION_NETWORK_NODE_H

#include "../proxy/network_layer/ip_proxy_server.h"
#include "onion_network_common.h"
#include "../temp_connections/udp_client_server/udp_server.h"

// onion network node. currently only works with one connection.
class onion_network_node {

    class udp_server udpServer;

    class server_app_handler : public basic_receiver<socket_msg>, public basic_connection {
    public:
        class udp_server* udpServer;

        server_app_handler(class udp_server* server) : udpServer(server) {
            udpServer->add_listener(this);
        }

        ip4_addr ip_source;
        int port_source;
        void handle_received_event(socket_msg& event) override {
            ip_source = event.source_addr;
            port_source = event.source_port;
            this->listenable::handle_received_event(event.msg);
        }

        int send_data(send_msg val) override {
            return udpServer->send_data_to_client(ip_source, port_source, val);
        }
    };
    server_app_handler udp_single_conn;

    ip_proxy_server proxy;

public:
    onion_network_node(basic_gateway *gw = nullptr) : proxy(gw), udpServer(ONION_NETWORK_NODE_LISTEN_PORT), udp_single_conn(&udpServer) {
        // add a single connection to the onion_network
        proxy.set_proxied_connection(&udp_single_conn);
    }
};


#endif //SERVERCLIENT_ONION_NETWORK_NODE_H
