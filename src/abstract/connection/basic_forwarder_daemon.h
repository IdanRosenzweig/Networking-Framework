#ifndef SERVERCLIENT_BASIC_FORWARDER_DAEMON_H
#define SERVERCLIENT_BASIC_FORWARDER_DAEMON_H

#include "../sending/msg_sender.h"
#include "../receiving/msg_receiver.h"
#include "basic_connection.h"
#include "basic_gateway.h"

class basic_forwarder_daemon;

class daemon_conn_side_handler;

class daemon_network_side_handler;

class daemon_conn_side_handler : public msg_sender, public msg_receiver {
public:
    basic_forwarder_daemon *daemon;

    int send_data(send_msg msg) override; // send to the client

    void handle_received_event(received_msg& msg) override; // recv from the client
};

class daemon_network_side_handler : public msg_sender, public msg_receiver {
public:
    basic_forwarder_daemon *daemon;

    int send_data(send_msg msg) override; // send to the network

    void handle_received_event(received_msg& msg) override; // recv from the network
};

// aggregates multiple connections and connects them to a gateway
class basic_forwarder_daemon {
public:
    basic_connection *conn_to_client;

// gateway to the network
    basic_gateway *network_gateway;

    daemon_conn_side_handler conn_handler;
    daemon_network_side_handler network_handler;


    explicit basic_forwarder_daemon(basic_connection *connToClient, basic_gateway *gw = nullptr) : conn_to_client(
            connToClient), network_gateway(gw) {
        conn_handler.daemon = this;
        network_handler.daemon = this;

        conn_to_client->add_listener(&conn_handler);
        network_gateway->add_listener(&network_handler);
    }
};



#endif //SERVERCLIENT_BASIC_FORWARDER_DAEMON_H
