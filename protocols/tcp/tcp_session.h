#ifndef SERVERCLIENT_TCP_SESSION_H
#define SERVERCLIENT_TCP_SESSION_H

#include <unistd.h>
#include <thread>

#include "../../abstract/basic_session.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"

class tcp_session : public basic_session {

    int sd;
    std::thread worker;

public:

    ip4_addr dest_addr;
    int source_port;
    int dest_port;

    tcp_session(int sd, ip4_addr destAddr, int sourcePort, int destPort);

    ~tcp_session();

    int send_data(send_msg msg) override;

    void handle_received_event(received_msg event) override;
};


#endif //SERVERCLIENT_TCP_SESSION_H
