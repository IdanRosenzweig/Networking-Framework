#include "tcp_boundary_preserving_server.h"

void tcp_boundary_preserving_server::sessions_handler::handle_callback(tcp_session_type &&data) {
    master->generate_session(
            std::move(tcp_boundary_preserving_session_type(data.sess_data,
                                                           std::make_unique<tcp_boundary_preserving_session_conn>(
                                                                   std::move(data.sess_conn))
            ))
    );
}

tcp_boundary_preserving_server::tcp_boundary_preserving_server(uint16_t port) : raw_tcp_server(port), sessionsHandler(this) {

}

tcp_boundary_preserving_session_type tcp_boundary_preserving_server::start_session(const string &ip, uint16_t port) {
    tcp_session_type raw_sess = raw_tcp_server.start_session(ip, port);
    return tcp_boundary_preserving_session_type(raw_sess.sess_data,
                                                std::make_unique<tcp_boundary_preserving_session_conn>(
                                                        std::move(raw_sess.sess_conn))
    );
}
