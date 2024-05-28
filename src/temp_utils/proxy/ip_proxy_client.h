#ifndef SERVERCLIENT_IP_PROXY_CLIENT_H
#define SERVERCLIENT_IP_PROXY_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../abstract/connection/connection.h"
#include "../../protocols/ip4/ip4_addr.h"
#include <iostream>
using namespace std;

// a client proxy for ip, providing a data_link_layer_gateway api
class ip_proxy_client : public gateway {
    connection* conn_to_proxy_server; // connection to the onion_network aggregator

public:
    explicit ip_proxy_client(connection *connToServer) : conn_to_proxy_server(connToServer) {
        conn_to_proxy_server->add_listener(this);
    }

    ~ip_proxy_client() {
        conn_to_proxy_server->remove_listener(this);
    }

    int send_data(send_msg<>&& msg) override {
        return conn_to_proxy_server->send_data(std::move(msg));
    }

    void handle_received_event(received_msg&& msg) override {
        this->recv_forwarder::handle_received_event(std::move(msg));
    }
};

#endif //SERVERCLIENT_IP_PROXY_CLIENT_H
