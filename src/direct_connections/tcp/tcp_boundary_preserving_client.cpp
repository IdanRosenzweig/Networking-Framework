#include "tcp_boundary_preserving_client.h"

tcp_boundary_preserving_client::tcp_boundary_preserving_client(ip4_addr ip, uint16_t port, uint16_t my_port, const string& iface) : tcpClient(ip, port, my_port, iface),
                                                                                                               client(&tcpClient) {
    client.add_listener(this);
}

int tcp_boundary_preserving_client::send_data(send_msg_t &&data) {
    return client.send_data(std::move(data));
}
