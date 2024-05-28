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

        void handle_received_event(tcp_session_type &&event) override {
            master->generate_event(
                    std::move(tcp_boundary_preserving_session_type(event.sess_data,
                                                         std::make_unique<tcp_boundary_preserving_session_conn>(
                                                                 std::move(event.sess_conn))
                    ))
            );
        }
    } sessionsHandler;

public:
    tcp_boundary_preserving_server(int port) : raw_tcp_server(port), sessionsHandler(this) {

    }

    tcp_boundary_preserving_session_type start_session(const string &ip, int port) {
        tcp_session_type raw_sess = raw_tcp_server.start_session(ip, port);
        return tcp_boundary_preserving_session_type(raw_sess.sess_data,
                                                    std::make_unique<tcp_boundary_preserving_session_conn>(
                                                            std::move(raw_sess.sess_conn))
        );
    }

};

#endif //NETWORKING_TCP_BOUNDARY_PRESERVING_SERVER_H
