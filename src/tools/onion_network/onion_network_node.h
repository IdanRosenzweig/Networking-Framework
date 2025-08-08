#ifndef NETWORKING_ONION_NETWORK_NODE_H
#define NETWORKING_ONION_NETWORK_NODE_H

#include "../proxy/ip_proxy_server.h"
#include "../../direct_connections/udp/udp_server.h"
#include "../../direct_connections/emp/emp_server.h"
#include "onion_network_common.h"

// onion network node. currently only works with one connection.
class onion_network_node {

    ip_proxy_server proxy;

    class udp_server udpServer;

    class server_app_handler : public recv_listener<udp_packet_stack>, public connection {
    public:
        onion_network_node* master;

        explicit server_app_handler(onion_network_node *master);

        ip4_addr ip_source;
        uint16_t port_source;
        void handle_callback(udp_packet_stack&& data) override;

        int send_data(send_msg_t&& data) override;
    } udp_single_conn;

public:
    onion_network_node(ip4_addr src_ip, gateway* network_layer_gw);
};


#endif //NETWORKING_ONION_NETWORK_NODE_H
