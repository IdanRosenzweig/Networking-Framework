#ifndef SERVERCLIENT_TCP_CLIENT_H
#define SERVERCLIENT_TCP_CLIENT_H

#include "../../abstract/connection/msg_connection.h"
#include "../../abstract/gateway/basic_gateway.h"
#include "../../protocols/tcp/tcp_protocol.h"

class tcp_client : public msg_connection {
public:
    tcp_protocol tcp_prot;

    std::unique_ptr<tcp_session> session;
    std::thread worker;

    tcp_client(ip4_addr ip, int port, int my_port);

    int send_data(send_msg& val) override;
};


#endif //SERVERCLIENT_TCP_CLIENT_H
