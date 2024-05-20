#ifndef SERVERCLIENT_TCP_SERVER_H
#define SERVERCLIENT_TCP_SERVER_H

#include "../../protocols/tcp/tcp_protocol.h"
#include "../../abstract/session/basic_session_generator.h"

// (no real difference between tcp_protocol and tcp_server, because i didn't implement
// the tcp_client_server protocol but rather used the linux api for this, so there is no need for creating
// the protocol stack here, just use tcp_protocol directly)
class tcp_server : public basic_session_generator<tcp_session> {
    tcp_protocol tcp_prot;

    int server_port;

    class sessions_handler : public basic_receiver<std::unique_ptr<tcp_session>> {
        tcp_server *master;

    public:
        explicit sessions_handler(tcp_server *master) : master(master) {
            master->tcp_prot.add_listener(this);
        }

        void handle_received_event(unique_ptr<tcp_session> &event) override {
            if (event->sessionData.dest_port == master->server_port)
                master->handle_received_event(event);
        }
    };
    sessions_handler sessionsHandler;

public:
    tcp_server(int port) : tcp_prot(true), server_port(port), sessionsHandler(this) {

    }

    std::unique_ptr<tcp_session> start_session(const string& ip, int port);

};


#endif //SERVERCLIENT_TCP_SERVER_H
