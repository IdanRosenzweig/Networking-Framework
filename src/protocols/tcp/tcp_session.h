#ifndef SERVERCLIENT_TCP_SESSION_H
#define SERVERCLIENT_TCP_SESSION_H

#include <unistd.h>
#include <thread>

#include "../../abstract/session/msg_session.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"

struct tcp_session_data {
    ip4_addr dest_addr;
    int source_port;
    int dest_port;
};

class tcp_session : public msg_session {

    int sd;
    std::thread worker;

public:

    tcp_session_data sessionData;

    bool alive;

    tcp_session(int sd, tcp_session_data data);

    ~tcp_session();

    int send_data(send_msg& msg) override;

    void handle_received_event(received_msg& event) override;

};


#endif //SERVERCLIENT_TCP_SESSION_H
