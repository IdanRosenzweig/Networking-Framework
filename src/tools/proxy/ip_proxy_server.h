#ifndef NETWORKING_IP_PROXY_SERVER_H
#define NETWORKING_IP_PROXY_SERVER_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/gateway/gateway.h"

#include "../../protocols/ip4/ip4_protocol.h"

#include <set>
#include <cstring>
#include <memory>

class ip_proxy_server;

class conn_side_handler;
class network_side_handler;

class conn_side_handler : public msg_send_medium, public msg_recv_listener {
    connection * my_conn;

public:
    ip4_addr my_source = ip4_addr_empty;

    explicit conn_side_handler(connection *myConn) : my_conn(myConn) {
        my_conn->add_listener(this);
    }

    ~conn_side_handler() {
        my_conn->remove_listener(this);
    }

    ip_proxy_server *server;

    int send_data(send_msg_t&&data) override; // send to the raw_session

    void handle_callback(recv_msg_t&& data) override; // recv from the raw_session
};

class network_side_handler : public msg_send_medium, public msg_recv_listener {
public:
    ip_proxy_server *server;

    int send_data(send_msg_t&&data) override; // send to the network

    void handle_callback(recv_msg_t&& data) override; // recv from the network
};


// onion_network at the ip level (changes ip header)
// currently works with one connection. can be extended to multiple clients
class ip_proxy_server {
public:
//    vector<class conn_side_handler> client_handlers; // all connections to be proxied
    std::unique_ptr<conn_side_handler> conn_handler; // the connection to be proxied

    // data_link_layer_gateway to the network
    gateway *network_layer_gw;
    class network_side_handler network_handler;

    // onion_network mappings
    set<ip4_addr> mappings;

    // stored source
    ip4_addr server_ip;

    ip_proxy_server(ip4_addr src_ip, gateway* gw);

    ~ip_proxy_server();

    void set_proxied_connection(connection* conn) {
        conn_handler = std::make_unique<conn_side_handler>(conn);
        conn_handler->server = this;
    }

};




#endif //NETWORKING_IP_PROXY_SERVER_H
