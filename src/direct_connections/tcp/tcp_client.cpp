#include "tcp_client.h"

tcp_client::tcp_client(ip4_addr dest_ip, uint16_t dest_port, uint16_t my_port, const string& iface) : tcp_prot(false, dest_port, iface) {
    tcp_prot.next_addr.set_next_choice(dest_ip);
    tcp_prot.next_dest_port.set_next_choice(dest_port);
    tcp_prot.next_source_port.set_next_choice(my_port);

    session = std::move(tcp_prot.start_session());
    session.sess_conn->add_listener(this);
}

int tcp_client::send_data(send_msg_t &&data) {
    return session.sess_conn->send_data(std::move(data));
}
