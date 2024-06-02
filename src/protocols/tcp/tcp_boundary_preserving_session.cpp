#include "tcp_boundary_preserving_session.h"

tcp_boundary_preserving_session_conn::tcp_boundary_preserving_session_conn(unique_ptr<tcp_session_conn> &&session) : raw_session(std::move(session)),
                                                                                                                     client(raw_session.get()) {
    client.add_listener(this);
}

int tcp_boundary_preserving_session_conn::send_data(send_msg_t &&data) {
    return client.send_data(std::move(data));
}
