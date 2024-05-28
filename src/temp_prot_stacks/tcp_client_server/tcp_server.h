#ifndef SERVERCLIENT_TCP_SERVER_H
#define SERVERCLIENT_TCP_SERVER_H

#include "../../protocols/tcp/tcp_protocol.h"
#include "../../abstract/session/session_generator.h"

// (no real difference between tcp_protocol and tcp_server, because i didn't implement
// the tcp protocol but rather used the linux api for this, so there is no need for creating
// the protocol stack here, just use tcp_protocol directly)
class tcp_server : public session_generator<tcp_session_type> {
    tcp_protocol tcp_prot;

    int server_port;

    class sessions_handler : public basic_recv_listener<tcp_session_type> {
        tcp_server *master;

    public:
        explicit sessions_handler(tcp_server *master) : master(master) {
            master->tcp_prot.set_generator_listener(this);
        }

        void handle_received_event(tcp_session_type &&event) override {
            if (event.sess_data.dest_port == master->server_port) {
                master->generate_event(std::move(event));
            }
        }
    };
    sessions_handler sessionsHandler;

public:
    tcp_server(int port) : tcp_prot(true), server_port(port), sessionsHandler(this) {

    }

    tcp_session_type start_session(const string& ip, int port);

};


#endif //SERVERCLIENT_TCP_SERVER_H
