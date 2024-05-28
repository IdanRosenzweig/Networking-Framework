#include "udp_client.h"

udp_client::udp_client(ip4_addr dest_ip, int dest_port, int my_port, ip4_addr src_ip, gateway *gw) : network_layer_gw(gw) {
    // setup send flow
    ip_client.send_medium = network_layer_gw;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(dest_ip);
    ip_client.next_source_addr.set_next_choice(src_ip);

    _udp_client.send_medium = &ip_client;
    _udp_client.next_source_port.set_next_choice(my_port);
    _udp_client.next_dest_port.set_next_choice(dest_port);

    // setup recv flow
    network_layer_gw->add_listener(&ip_client);

    // todo change to both udp_client_server and raw_session matching
    ip_client.protocol_handlers[IPPROTO_UDP].push_back( &_udp_client);

    _udp_client.port_handlers[my_port].push_back(this);
}

udp_client::~udp_client() {
    network_layer_gw->remove_listener(&ip_client);
}

int udp_client::send_data(send_msg<> &&msg) {
    return _udp_client.send_data(std::move(msg));
}
