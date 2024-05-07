#ifndef SERVERCLIENT_IP_PROXY_CLIENT_H
#define SERVERCLIENT_IP_PROXY_CLIENT_H

#include "../../abstract/connection/basic_gateway.h"
#include "../../protocols/ip4/ip4_addr.h"
#include <iostream>
using namespace std;

// a onion_network for ip, providing a gateway api
class ip_proxy_client : public basic_gateway {
    basic_connection* conn_to_proxy_server; // connection to the onion_network aggregator

public:
    explicit ip_proxy_client(basic_connection *connToServer) : conn_to_proxy_server(connToServer) {
        conn_to_proxy_server->add_listener(this);
    }

    int send_data(send_msg msg) override {
        return conn_to_proxy_server->send_data(msg);
    }

    void handle_received_event(received_msg& msg) override {
        this->listenable::handle_received_event(msg);
    }
};

#endif //SERVERCLIENT_IP_PROXY_CLIENT_H
