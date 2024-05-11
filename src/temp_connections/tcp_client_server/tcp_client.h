#ifndef SERVERCLIENT_TCP_CLIENT_H
#define SERVERCLIENT_TCP_CLIENT_H

#include "../../abstract/connection/basic_connection.h"
#include "../../abstract/gateway/basic_gateway.h"
#include "../../protocols/tcp/tcp_protocol.h"

class tcp_client : public basic_connection {
public:
    tcp_protocol tcp_prot;

    std::unique_ptr<tcp_session> session;
    std::thread worker;

    tcp_client(ip4_addr ip, int port, int my_port) : tcp_prot(false) {
        tcp_prot.next_addr.set_next_choice(ip);
        tcp_prot.next_dest_port.set_next_choice(port);
        tcp_prot.next_source_port.set_next_choice(my_port);

        session = std::move(tcp_prot.start_session());

        session->add_listener(this);
    }

    int send_data(send_msg val) override {
        return session->send_data(val);
    }
};


#endif //SERVERCLIENT_TCP_CLIENT_H
