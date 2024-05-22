#ifndef SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
#define SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H

#include "tcp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../abstract/session/session_generator.h"

class tcp_boundary_preserving_session : public session_conn {
    std::unique_ptr<tcp_session> raw_session;
    msg_boundary_seperator<> client;

public:
    tcp_boundary_preserving_session(std::unique_ptr<tcp_session> &&session) : raw_session(std::move(session)),
                                                                              client(raw_session.get()) {
        client.add_listener(this);
    }

    int send_data(send_msg<> &&val) override {
        return client.send_data(std::move(val));
    }

    const tcp_session_data &get_tcp_session_data() {
        return raw_session->sessionData;
    }

};

class tcp_boundary_preserving_server : public session_generator<tcp_boundary_preserving_session> {
    tcp_server raw_tcp_server;

    class sessions_handler : public basic_receiver<session_t<tcp_session>> {
        tcp_boundary_preserving_server *master;

    public:
        explicit sessions_handler(tcp_boundary_preserving_server *master) : master(master) {
            master->raw_tcp_server.set_generator_listener(this);
        }

        void handle_received_event(session_t<tcp_session> &&event) override {
            std::unique_ptr<tcp_boundary_preserving_session> wrapper = std::make_unique<tcp_boundary_preserving_session>(
                    std::move(event.session));
            master->generate_event(session_t<tcp_boundary_preserving_session>(event.id, std::move(wrapper)));
        }
    };

    sessions_handler sessionsHandler;

public:
    tcp_boundary_preserving_server(int port) : raw_tcp_server(port), sessionsHandler(this) {

    }

    session_t<tcp_boundary_preserving_session> start_session(const string &ip, int port) {
        session_t<tcp_session> raw_sess = raw_tcp_server.start_session(ip, port);
        return {raw_sess.id,
                std::make_unique<tcp_boundary_preserving_session>(std::move(raw_sess.session))
        };
    }

};

#endif //SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
