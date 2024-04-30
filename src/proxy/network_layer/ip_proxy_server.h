#ifndef SERVERCLIENT_IP_PROXY_SERVER_H
#define SERVERCLIENT_IP_PROXY_SERVER_H

#include "../../abstract/connection/basic_connection.h"
#include "../../abstract/connection/basic_gateway.h"
#include "../../abstract/receiving/msg_receiver.h"

#include "../../linux/network_layer_gateway/network_layer_gateway.h"

#include "../../protocols/ip4/ip4_addr.h"

#include <set>
#include <cstring>
#include <arpa/inet.h>
#include <linux/ip.h>

class ip_proxy_server;

class conn_side_handler;
class network_side_handler;

class conn_side_handler : public msg_sender, public msg_receiver {
public:
    ip_proxy_server *server;

    int send_data(send_msg msg) override; // send to the client

    void handle_received_event(received_msg& msg) override; // recv from the client
};

class network_side_handler : public msg_sender, public msg_receiver {
public:
    ip_proxy_server *server;

    int send_data(send_msg msg) override; // send to the network

    void handle_received_event(received_msg& msg) override; // recv from the network
};


// proxy at the ip level (changes ip header)
class ip_proxy_server {
public:
    // connection to be proxied
    basic_connection *connection;

    // gateway to the network
    basic_gateway *network_layer_gateway;

    class conn_side_handler client_handler;
    class network_side_handler network_handler;

    // proxy mappings
    set<ip4_addr> mappings;

    // stored source
    ip4_addr source;

    ip4_addr my_ip;

    ip_proxy_server() = delete;

    ip_proxy_server(basic_connection *conn, basic_gateway *gw = nullptr) : connection(conn) {
        my_ip = get_my_priv_ip_addr("enp0s3");

        client_handler.server = this;
        network_handler.server = this;

        if (gw == nullptr) {
            network_layer_gateway = new class network_layer_gateway;
        } else
            network_layer_gateway = gw;

        // client side
        connection->add_listener(&client_handler); // recv
        // no need to assign class for send, conn_side_handler would just use this class's reference

        // recv from network
        network_layer_gateway->add_listener(&network_handler);
        // no need to assign class for send, network_side_handler would just use this class's reference
    }

};




#endif //SERVERCLIENT_IP_PROXY_SERVER_H
