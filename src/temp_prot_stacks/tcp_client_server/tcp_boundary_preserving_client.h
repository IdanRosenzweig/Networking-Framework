#ifndef NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H
#define NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H

#include "tcp_client.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../protocols/tcp/tcp_boundary_preserving_session.h"

class tcp_boundary_preserving_client : public connection {
public:
    tcp_client tcpClient;

    msg_boundary_seperator<> client;

    tcp_boundary_preserving_client(ip4_addr ip, int port, int my_port) : tcpClient(ip, port, my_port),
                                                                         client(&tcpClient) {
        client.add_listener(this);
    }

    int send_data(send_msg<> &&val) override {
        return client.send_data(std::move(val));
    }
};


#endif //NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H
