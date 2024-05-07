#ifndef SERVERCLIENT_TCP_PROTOCOL_H
#define SERVERCLIENT_TCP_PROTOCOL_H

#include <vector>
#include <cstring>
#include <thread>

#include "../../abstract/connection/basic_session.h"
#include "../../abstract/utils/next_choice.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"


#include "tcp_session.h"

class tcp_protocol : public listenable<std::unique_ptr<tcp_session>> {

    int sd;
    std::thread worker;

public:

    tcp_protocol(bool SERVER);

    ~tcp_protocol();

    next_choice<ip4_addr> next_addr;
    next_choice<int> next_dest_port;
    next_choice<int> next_source_port;

    std::unique_ptr<tcp_session> start_session();

};


#endif //SERVERCLIENT_TCP_PROTOCOL_H
