#ifndef SERVERCLIENT_TCP_BOUNDARY_PRESERVING_CLIENT_H
#define SERVERCLIENT_TCP_BOUNDARY_PRESERVING_CLIENT_H

#include "tcp_client.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"

class tcp_boundary_preserving_client : public basic_connection {
public:
    tcp_client tcpClient;
    msg_boundary_seperator<> client;

    tcp_boundary_preserving_client(ip4_addr ip, int port, int my_port) : tcpClient(ip, port, my_port), client(&tcpClient) {
        client.add_listener(this);
    }

    int send_data(send_msg val) override {
        return client.send_data(val);
    }
};


#endif //SERVERCLIENT_TCP_BOUNDARY_PRESERVING_CLIENT_H
