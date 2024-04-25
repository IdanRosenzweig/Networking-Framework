#ifndef SERVERCLIENT_PROXY_GATEWAY_CLIENT_H
#define SERVERCLIENT_PROXY_GATEWAY_CLIENT_H

#include "../abstract/basic_connection.h"
#include "../abstract/basic_gateway.h"
#include "../abstract/receiving/received_msg.h"
#include <iostream>
using namespace std;


class proxy_gateway_client : public basic_gateway {
public:
    basic_connection* conn_to_server; // connection to the proxy server

    explicit proxy_gateway_client(basic_connection *connToServer) : conn_to_server(connToServer) {
        conn_to_server->add_listener(this);
    }

    int send_data(send_msg msg) override {
        return conn_to_server->send_data(msg);
    }

    void handle_received_event(received_msg msg) override {
        this->listenable::handle_received_event(std::move(msg));
    }
};

#endif //SERVERCLIENT_PROXY_GATEWAY_CLIENT_H
