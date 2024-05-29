#include "tcp_server.h"

tcp_session_type tcp_server::start_session(const string& ip, uint16_t port) {
    tcp_prot.next_addr.set_next_choice(convert_to_ip4_addr(ip));
    tcp_prot.next_dest_port.set_next_choice(port);
    tcp_prot.next_source_port.set_next_choice(5656);

    tcp_session_type session = tcp_prot.start_session();
    return std::move(session);
}
