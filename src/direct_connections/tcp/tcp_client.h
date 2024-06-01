#ifndef NETWORKING_TCP_CLIENT_H
#define NETWORKING_TCP_CLIENT_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"
#include "../../protocols/tcp/tcp_protocol.h"

class tcp_client : public connection {
public:
    tcp_protocol tcp_prot;

    tcp_session_type session;

    tcp_client(ip4_addr ip, uint16_t port, uint16_t my_port);

    int send_data(send_msg_t&& data) override;

};


#endif //NETWORKING_TCP_CLIENT_H
