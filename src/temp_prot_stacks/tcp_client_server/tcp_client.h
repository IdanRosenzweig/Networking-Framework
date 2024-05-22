#ifndef SERVERCLIENT_TCP_CLIENT_H
#define SERVERCLIENT_TCP_CLIENT_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"
#include "../../protocols/tcp/tcp_protocol.h"

class tcp_client : public connection {
public:
    tcp_protocol tcp_prot;

    session_t<tcp_session> session;

    tcp_client(ip4_addr ip, int port, int my_port);

    int send_data(send_msg<>&& val) override;

};


#endif //SERVERCLIENT_TCP_CLIENT_H
