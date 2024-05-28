#ifndef SERVERCLIENT_ONION_NETWORK_NODE_H
#define SERVERCLIENT_ONION_NETWORK_NODE_H

#include "../proxy/ip_proxy_server.h"
#include "onion_network_common.h"
#include "../../temp_prot_stacks/udp_client_server/udp_server.h"
#include "../../temp_prot_stacks/emp/emp_server.h"

// onion network node. currently only works with one connection.
class onion_network_node {

    ip_proxy_server proxy;

    class udp_server udpServer;

    class server_app_handler : public basic_recv_listener<udp_packet_stack>, public connection {
    public:
        onion_network_node* master;

        explicit server_app_handler(onion_network_node *master) : master(master) {
            master->udpServer.add_listener(this);
        }

        ip4_addr ip_source;
        int port_source;
        void handle_received_event(udp_packet_stack&& event) override {
            ip_source = event.source_addr;
            port_source = event.source_port;
            this->recv_forwarder::handle_received_event(std::move(event.msg));
        }

        int send_data(send_msg<>&& val) override {
            return master->udpServer.send_data_to_client(ip_source, port_source, val);
        }
    };
    server_app_handler udp_single_conn;

public:
    onion_network_node(ip4_addr src_ip, gateway* network_layer_gw) : proxy(src_ip, network_layer_gw), udpServer(ONION_NETWORK_NODE_LISTEN_PORT, src_ip, network_layer_gw), udp_single_conn(this) {
        // add a single connection to the onion_network
        proxy.set_proxied_connection(&udp_single_conn);
    }
};


#endif //SERVERCLIENT_ONION_NETWORK_NODE_H
