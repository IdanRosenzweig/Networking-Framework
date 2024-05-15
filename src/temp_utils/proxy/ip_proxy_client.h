#ifndef SERVERCLIENT_IP_PROXY_CLIENT_H
#define SERVERCLIENT_IP_PROXY_CLIENT_H

#include "../../abstract/gateway/msg_gateway.h"
#include "../../abstract/connection/msg_connection.h"
#include "../../protocols/ip4/ip4_addr.h"
#include <iostream>
using namespace std;

// a client proxy for ip, providing a gateway api
class ip_proxy_client : public msg_gateway {
    msg_connection* conn_to_proxy_server; // connection to the onion_network aggregator

public:
    explicit ip_proxy_client(msg_connection *connToServer) : conn_to_proxy_server(connToServer) {
        conn_to_proxy_server->add_listener(this);
    }

    int send_data(send_msg& msg) override {
        return conn_to_proxy_server->send_data(msg);
    }

    void handle_received_event(received_msg& msg) override {
        this->multi_receiver::handle_received_event(msg);
    }
};

#endif //SERVERCLIENT_IP_PROXY_CLIENT_H
