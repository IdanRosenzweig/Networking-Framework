#ifndef NETWORKING_TCP_BOUNDARY_PRESERVING_SESSION_H
#define NETWORKING_TCP_BOUNDARY_PRESERVING_SESSION_H

#include "tcp_session.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../abstract/session/session_t.h"

class tcp_boundary_preserving_session_conn : public session_conn {
    std::unique_ptr<tcp_session_conn> raw_session;
    msg_boundary_seperator<> client;

public:
    tcp_boundary_preserving_session_conn(std::unique_ptr<tcp_session_conn> &&session) : raw_session(std::move(session)),
                                                                                        client(raw_session.get()) {
        client.add_listener(this);
    }

    int send_data(send_msg<> &&val) override {
        return client.send_data(std::move(val));
    }

};

using tcp_boundary_preserving_session_type = session_t<tcp_boundary_preserving_session_conn, tcp_session_data>;

#endif //NETWORKING_TCP_BOUNDARY_PRESERVING_SESSION_H
