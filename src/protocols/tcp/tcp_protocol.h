#ifndef NETWORKING_TCP_PROTOCOL_H
#define NETWORKING_TCP_PROTOCOL_H

#include <vector>
#include <cstring>
#include <thread>

#include "../../abstract/session/session_conn.h"
#include "../../abstract/session/session_generator.h"
#include "../../utils/next_choice.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"

#include "tcp_session.h"

using tcp_session_type = session_t<tcp_session_conn, tcp_session_data>;

class tcp_protocol : public session_generator<tcp_session_type> {
    string local_iface;

    bool need_server;
    uint16_t server_port;
    int sd;
    std::thread worker;

public:

    tcp_protocol(bool need_server, uint16_t port, const string& iface);

    ~tcp_protocol();

    next_choice<ip4_addr> next_addr;
    next_choice<uint16_t> next_dest_port;
    next_choice<uint16_t> next_source_port;

    tcp_session_type start_session();

};


#endif //NETWORKING_TCP_PROTOCOL_H
