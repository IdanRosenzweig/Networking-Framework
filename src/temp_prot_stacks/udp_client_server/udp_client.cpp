#include "udp_client.h"

udp_client::udp_client(ip4_addr dest_ip, uint16_t dest_port, uint16_t my_port, ip4_addr src_ip, gateway *gw) : network_layer_gw(gw) {
    // setup send flow
    ip_prot.send_medium.add_send_channel(network_layer_gw);
    ip_prot.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_prot.next_dest_addr.set_next_choice(dest_ip);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    udp_prot.send_medium.add_send_channel(&ip_prot);
    udp_prot.next_source_port.set_next_choice(my_port);
    udp_prot.next_dest_port.set_next_choice(dest_port);

    // setup recv flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.listeners.append_new_empty_handler(&udp_prot);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_UDP);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_DEST_ADDR_INDEX>(src_ip);

    udp_prot.listeners.append_new_empty_handler(this);
    udp_prot.listeners.add_requirement_to_last_handler<UDP_LISTEN_ON_PORT_INDEX>(my_port);
}

udp_client::~udp_client() {
    network_layer_gw->remove_listener(&ip_prot);
}

int udp_client::send_data(send_msg<> &&msg) {
    return udp_prot.send_data(std::move(msg));
}
