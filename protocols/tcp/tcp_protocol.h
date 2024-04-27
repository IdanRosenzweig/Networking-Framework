#ifndef SERVERCLIENT_TCP_PROTOCOL_H
#define SERVERCLIENT_TCP_PROTOCOL_H

#include <vector>
#include <cstring>
#include <thread>

#include "../../abstract/connection/basic_session.h"
#include "../../abstract/next_choice.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"


#include "tcp_session.h"

class tcp_protocol {

    int sd;
    std::thread worker;

public:

    std::vector<std::unique_ptr<basic_session>> as_server_sessions; // sessions where i am the server

    tcp_protocol();

    ~tcp_protocol();


    std::vector<std::unique_ptr<basic_session>> as_client_sessions; // sessions where i am the client

    next_choice<ip4_addr> next_addr;
    next_choice<int> next_dest_port;
    next_choice<int> next_source_port;

    void start_session();

};


#endif //SERVERCLIENT_TCP_PROTOCOL_H
