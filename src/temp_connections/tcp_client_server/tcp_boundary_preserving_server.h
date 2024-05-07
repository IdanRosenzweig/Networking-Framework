#ifndef SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
#define SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H

#include "tcp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"

class boundary_preserving_session : public basic_session {
    std::unique_ptr<tcp_session> tcpSession;
    msg_boundary_seperator<> client;

public:
    boundary_preserving_session(unique_ptr<tcp_session> &session) : tcpSession(std::move(session)), client(tcpSession.get()) {
        client.add_listener(this);
    }

    int send_data(send_msg val) override {
        return client.send_data(val);
    }

    tcp_session_data get_tcp_session_data() {
        return tcpSession->sessionData;
    }

};

class tcp_boundary_preserving_server : public listenable<std::unique_ptr<basic_session>> {
    tcp_server server;


    class sessions_handler : public basic_receiver<std::unique_ptr<tcp_session>> {
        tcp_boundary_preserving_server *master;

    public:
        explicit sessions_handler(tcp_boundary_preserving_server *master) : master(master) {
            master->server.add_listener(this);
        }

        void handle_received_event(unique_ptr<tcp_session> &event) override {
                std::unique_ptr<basic_session> wrapper = std::make_unique<boundary_preserving_session>(event);
                master->handle_received_event(wrapper);
        }
    };

    sessions_handler sessionsHandler;

    vector<std::unique_ptr<tcp_session>> tcpSession;
    vector<msg_boundary_seperator<>> sessions;

public:
    tcp_boundary_preserving_server(int port) : server(port), sessionsHandler(this) {

    }

    std::unique_ptr<basic_session> start_session(const string &ip, int port) {
        return std::move(server.start_session(ip, port));
    }

};

#endif //SERVERCLIENT_TCP_BOUNDARY_PRESERVING_SERVER_H
