#include "onion_network_node.h"

onion_network_node::server_app_handler::server_app_handler(onion_network_node *master) : master(master) {
    master->udpServer.add_listener(this);
}

void onion_network_node::server_app_handler::handle_callback(udp_packet_stack &&data) {
    ip_source = data.source_addr;
    port_source = data.source_port;
    this->recv_forwarder::handle_callback(std::move(data.plain_data));
}

int onion_network_node::server_app_handler::send_data(send_msg_t &&data) {
    return master->udpServer.send_data_to_client(ip_source, port_source, std::move(data));
}

onion_network_node::onion_network_node(ip4_addr src_ip, gateway *network_layer_gw) : proxy(src_ip, network_layer_gw), udpServer(ONION_NETWORK_NODE_LISTEN_PORT, src_ip, network_layer_gw), udp_single_conn(this) {
    // add a single connection to the onion_network
    proxy.set_proxied_connection(&udp_single_conn);
}
