#ifndef SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
#define SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H

#include "tcp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../abstract/session/basic_session_generator.h"

class tcp_boundary_preserving_session : public msg_session {
    std::unique_ptr<tcp_session> tcpSession;
    msg_boundary_seperator<> client;

public:
    tcp_boundary_preserving_session(unique_ptr<tcp_session> &&session) : tcpSession(std::move(session)),
                                                                        client(tcpSession.get()) {
        client.add_listener(this);
    }

    int send_data(send_msg<>& val) override {
        return client.send_data(val);
    }

    const tcp_session_data& get_tcp_session_data() {
        return tcpSession->sessionData;
    }

};

class tcp_boundary_preserving_server : public basic_session_generator<tcp_boundary_preserving_session> {
    tcp_server raw_tcp_server;

    class sessions_handler : public basic_receiver<std::unique_ptr<tcp_session>> {
        tcp_boundary_preserving_server *master;

    public:
        explicit sessions_handler(tcp_boundary_preserving_server *master) : master(master) {
            master->raw_tcp_server.add_listener(this);
        }

        void handle_received_event(unique_ptr<tcp_session> &event) override {
            std::unique_ptr<tcp_boundary_preserving_session> wrapper = std::make_unique<tcp_boundary_preserving_session>(std::move(event));
            master->handle_received_event(wrapper);
        }
    };
    sessions_handler sessionsHandler;

public:
    tcp_boundary_preserving_server(int port) : raw_tcp_server(port), sessionsHandler(this) {

    }

    std::unique_ptr<tcp_boundary_preserving_session> start_session(const string &ip, int port) {
        std::unique_ptr<tcp_session> raw_sess = std::move(raw_tcp_server.start_session(ip, port));
        return std::make_unique<tcp_boundary_preserving_session>(std::move(raw_sess));
    }

};

#endif //SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
