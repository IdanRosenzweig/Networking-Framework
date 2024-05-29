#include "icmp_connection_client.h"

icmp_connection_client::icmp_connection_client(ip4_addr dest_ip, ip4_addr src_ip, gateway * gw) : network_layer_gw(gw) {
    // setup send flow
    ip_prot.send_medium.add_send_channel(network_layer_gw);
    ip_prot.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_prot.next_dest_addr.set_next_choice(dest_ip);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    icmp_prot.send_medium.add_send_channel(&ip_prot);
    icmp_prot.next_type.set_next_choice(ICMP_CONN_CLIENT_TYPE);
    icmp_prot.next_code.set_next_choice(ICMP_CONN_CLIENT_CODE);

    // setup recv flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.listeners.append_new_empty_handler(&icmp_prot);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_ICMP);

    icmp_prot.listeners.append_new_empty_handler(this);
    icmp_prot.listeners.add_requirement_to_last_handler<ICMP_LISTEN_ON_TYPE_INDEX>(ICMP_CONN_SERVER_TYPE);
}

icmp_connection_client::~icmp_connection_client() {
    network_layer_gw->remove_listener(&ip_prot);
}

int icmp_connection_client::send_data(send_msg<> &&msg) {
    return icmp_prot.send_data(std::move(msg));
}
