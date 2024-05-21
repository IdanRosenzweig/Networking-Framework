#include "udp_client.h"
#include "../../linux/hardware.h"

udp_client::udp_client(ip4_addr dest_ip, int dest_port, int my_port, ip4_addr src_ip, msg_gateway *network_layer_gw) {
    gateway = network_layer_gw;

    // setup send flow
    ip_client.gateway = gateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(dest_ip);
    ip_client.next_source_addr.set_next_choice(src_ip);

    _udp_client.gateway = &ip_client;
    _udp_client.next_source_port.set_next_choice(my_port);
    _udp_client.next_dest_port.set_next_choice(dest_port);

    // setup recv flow
    gateway->add_listener(&ip_client);

    // todo change to both udp_client_server and tcpSession matching
    ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_client);

    _udp_client.port_handlers.assign_to_key(my_port, this);
}

int udp_client::send_data(send_msg<> &msg) {
    return _udp_client.send_data(msg);
}
