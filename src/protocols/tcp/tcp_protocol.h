#ifndef SERVERCLIENT_TCP_PROTOCOL_H
#define SERVERCLIENT_TCP_PROTOCOL_H

#include <vector>
#include <cstring>
#include <thread>

#include "../../abstract/session/session_conn.h"
#include "../../abstract/session/session_generator.h"
#include "../../abstract/utils/next_choice.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"

#include "tcp_session.h"

using tcp_session_type = session_t<tcp_session_conn, tcp_session_data>;

//class tcp_protocol : public receive_multiplexer<std::unique_ptr<tcp_session>> {
class tcp_protocol : public session_generator<tcp_session_type> {

    int sd;
    std::thread worker;

public:

    tcp_protocol(bool SERVER);

    ~tcp_protocol();

    next_choice<ip4_addr> next_addr;
    next_choice<int> next_dest_port;
    next_choice<int> next_source_port;

    tcp_session_type start_session();

};


#endif //SERVERCLIENT_TCP_PROTOCOL_H
