#include "tcp_server.h"

session_t<tcp_session> tcp_server::start_session(const string& ip, int port) {
    tcp_prot.next_addr.set_next_choice(convert_to_ip4_addr(ip));
    tcp_prot.next_dest_port.set_next_choice(port);
    tcp_prot.next_source_port.set_next_choice(5656);
    return std::move(tcp_prot.start_session());
}
