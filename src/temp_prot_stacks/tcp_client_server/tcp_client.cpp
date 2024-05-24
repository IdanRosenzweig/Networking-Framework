#include "tcp_client.h"

tcp_client::tcp_client(ip4_addr ip, int port, int my_port) : tcp_prot(false) {
    tcp_prot.next_addr.set_next_choice(ip);
    tcp_prot.next_dest_port.set_next_choice(port);
    tcp_prot.next_source_port.set_next_choice(my_port);

    session = std::move(tcp_prot.start_session());
    session.sess_conn->add_listener(this);
}

int tcp_client::send_data(send_msg<> &&val) {
    return session.sess_conn->send_data(std::move(val));
}
