#include "icmp_connection_client.h"

icmp_connection_client::icmp_connection_client(ip4_addr dest_ip, ip4_addr src_ip, msg_gateway * network_layer_gw) {
    gateway = network_layer_gw;

    // setup send flow
    ip_client.gateway = gateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_client.next_dest_addr.set_next_choice(dest_ip);
    ip_client.next_source_addr.set_next_choice(src_ip);

    icmp_client.gateway = &ip_client;
    icmp_client.next_type.set_next_choice(ICMP_CONN_CLIENT_TYPE);
    icmp_client.next_code.set_next_choice(ICMP_CONN_CLIENT_CODE);

    // setup recv flow
    gateway->add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

    icmp_client.type_handlers.assign_to_key(ICMP_CONN_SERVER_TYPE, this);
}

int icmp_connection_client::send_data(send_msg<> &msg) {
    return icmp_client.send_data(msg);
}
