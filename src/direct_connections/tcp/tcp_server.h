#ifndef NETWORKING_TCP_SERVER_H
#define NETWORKING_TCP_SERVER_H

#include "../../protocols/tcp/tcp_protocol.h"
#include "../../abstract/session/session_generator.h"

// (no real difference between tcp_protocol and tcp_server, because i didn't implement
// the tcp protocol but rather used the linux api for this, so there is no need for creating
// the protocol stack here, just use tcp_protocol directly)
class tcp_server : public session_generator<tcp_session_type> {
    tcp_protocol tcp_prot;

    uint16_t server_port;

    class sessions_handler : public basic_recv_listener<tcp_session_type> {
        tcp_server *master;

    public:
        explicit sessions_handler(tcp_server *master) : master(master) {
            master->tcp_prot.set_generator_listener(this);
        }

        void handle_callback(tcp_session_type &&data) override {
            if (data.sess_data.dest_port == master->server_port) {
                master->generate_session(std::move(data));
            }
        }
    };
    sessions_handler sessionsHandler;

public:
    tcp_server(uint16_t port, const string& iface) : tcp_prot(true, port, iface), server_port(port), sessionsHandler(this) {

    }

    tcp_session_type start_session(const string& ip, uint16_t port);

};


#endif //NETWORKING_TCP_SERVER_H
