#ifndef SERVERCLIENT_IP_PROXY_H
#define SERVERCLIENT_IP_PROXY_H

#include <netinet/in.h>
#include "../abstract/basic_connection.h"

#include <set>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <linux/ip.h>


class ip_proxy;

class conn_side_handler;
class network_side_handler;

class conn_side_handler : public msg_sender, public msg_receiver {
public:
    ip_proxy *server;

    int send_data(send_msg msg) override; // send to the client

    void handle_received_event(received_msg msg) override; // recv from the client
};

class network_side_handler : public msg_sender, public msg_receiver {
public:
    ip_proxy *server;

    int send_data(send_msg msg) override; // send to the network

    void handle_received_event(received_msg msg) override; // recv from the network
};


// proxy at the ip level (changes ip header)
class ip_proxy {
public:
    // connection to be proxied
    basic_connection *connection;

    // gateway to the network
    basic_gateway *network_gateway;

    class conn_side_handler client_handler;
    class network_side_handler network_handler;

    // proxy mappings
    set<ip4_addr> mappings;

    ip_proxy() = delete;

    ip_proxy(basic_connection *conn, basic_gateway *gw = nullptr) : connection(conn) {
        if (gw == nullptr) {
            network_gateway = new network_layer_gateway;
        } else
            network_gateway = gw;

        // setup send to client flow
        // client handler would automatically use _udp_server.

        // setup recv from client flow
        connection->add_listener(&client_handler);

        // give handlers a reference
        client_handler.server = this;
        network_handler.server = this;
        network_gateway->add_listener(&network_handler);
    }

};


int conn_side_handler::send_data(send_msg msg) {
    return server->connection->send_data(msg);
}

void conn_side_handler::handle_received_event(received_msg msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    cout << "received proxied packet, size " << cnt << endl;
    if (cnt <= 0) return;

    // change source ip address
    ((struct iphdr *) buff)->saddr = inet_addr("10.100.102.18");

    // add dest ip addr to mapping
    // todo this can also store the original ip address is case of many clients using the proxy server
    server->mappings.insert(ip4_addr{((struct iphdr *) buff)->daddr});

    cout << "sending raw bytes to ip level" << endl;
    server->network_handler.send_data({buff, cnt});
}

int network_side_handler::send_data(send_msg msg) {
    return server->network_gateway->send_data(msg);
}

void network_side_handler::handle_received_event(received_msg msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    if (cnt <= 0) return;

    // ensure ip is in mapping
    ip4_addr addr{((struct iphdr *) buff)->saddr};
    if (!server->mappings.count(addr)) return;

    cout << "sending reply back to client" << endl;

    // change dest ip address
    // todo this addrsese can be taken from the stored one in the mapping
    ((struct iphdr *) buff)->daddr = inet_addr("10.100.102.18");

    server->client_handler.send_data({buff, cnt});
}

#endif //SERVERCLIENT_IP_PROXY_H
