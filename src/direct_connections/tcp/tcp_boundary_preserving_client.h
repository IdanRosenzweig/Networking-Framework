#ifndef NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H
#define NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H

#include "tcp_client.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../protocols/tcp/tcp_boundary_preserving_session.h"

class tcp_boundary_preserving_client : public connection {
public:
    tcp_client tcpClient;

    msg_boundary_seperator<> client;

    tcp_boundary_preserving_client(ip4_addr ip, uint16_t port, uint16_t my_port);

    int send_data(send_msg_t &&data) override;
};


#endif //NETWORKING_TCP_BOUNDARY_PRESERVING_CLIENT_H
