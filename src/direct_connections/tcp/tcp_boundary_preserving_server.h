#ifndef NETWORKING_TCP_BOUNDARY_PRESERVING_SERVER_H
#define NETWORKING_TCP_BOUNDARY_PRESERVING_SERVER_H

#include "tcp_server.h"
#include "../../protocols/tcp/tcp_boundary_preserving_session.h"
#include "../../abstract/session/session_generator.h"


class tcp_boundary_preserving_server : public session_generator<tcp_boundary_preserving_session_type> {
    tcp_server raw_tcp_server;

    class sessions_handler : public basic_recv_listener<tcp_session_type> {
        tcp_boundary_preserving_server *master;
    public:
        explicit sessions_handler(tcp_boundary_preserving_server *master) : master(master) {
            master->raw_tcp_server.set_generator_listener(this);
        }

        void handle_callback(tcp_session_type &&data) override;
    } sessionsHandler;

public:
    tcp_boundary_preserving_server(uint16_t port);

    tcp_boundary_preserving_session_type start_session(const string &ip, uint16_t port);

};

#endif //NETWORKING_TCP_BOUNDARY_PRESERVING_SERVER_H
