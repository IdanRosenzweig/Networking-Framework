#ifndef SERVERCLIENT_IP_PROXY_SERVER_H
#define SERVERCLIENT_IP_PROXY_SERVER_H

#include "../../abstract/connection/msg_connection.h"
#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/gateway/msg_gateway.h"

#include "../../protocols/ip4/ip4_addr.h"

#include <set>
#include <cstring>
#include <arpa/inet.h>

class ip_proxy_server;

class conn_side_handler;
class network_side_handler;

class conn_side_handler : public msg_sender, public msg_receiver {
    msg_connection * my_conn;

public:
    ip4_addr my_source = empty_ip4_addr;

    explicit conn_side_handler(msg_connection *myConn) : my_conn(myConn) {
        my_conn->add_listener(this);
    }

    ip_proxy_server *server;

    int send_data(send_msg<>& msg) override; // send to the tcpSession

    void handle_received_event(received_msg& msg) override; // recv from the tcpSession
};

class network_side_handler : public msg_sender, public msg_receiver {
public:
    ip_proxy_server *server;

    int send_data(send_msg<>& msg) override; // send to the network

    void handle_received_event(received_msg& msg) override; // recv from the network
};


// onion_network at the ip level (changes ip header)
// currently works with one connection. can be extended to multiple clients
class ip_proxy_server {
public:
//    vector<class conn_side_handler> client_handlers; // all connections to be proxied
    std::unique_ptr<conn_side_handler> conn_handler; // the connection to be proxied

    // gateway to the network
    msg_gateway *network_layer_gateway;
    class network_side_handler network_handler;

    // onion_network mappings
//    map<ip4_addr, set<conn_side_handler*>> mappings;
    set<ip4_addr> mappings;

    // stored source
    ip4_addr server_ip;

    ip_proxy_server(ip4_addr src_ip, msg_gateway* network_layer_gw);

//    void set_proxied_connection(msg_connection* conn) {
    void set_proxied_connection(msg_connection* conn) {
        conn_handler = std::make_unique<conn_side_handler>(conn);
        conn_handler->server = this;
//        conn_side_handler app_handler(conn);
//        app_handler.raw_tcp_server = this;
//        client_handlers.push_back(std::move(app_handler));
    }

};




#endif //SERVERCLIENT_IP_PROXY_SERVER_H
