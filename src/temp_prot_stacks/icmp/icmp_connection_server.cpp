#include "icmp_connection_server.h"

icmp_connection_server::icmp_connection_server(ip4_addr dest_ip, ip4_addr src_ip, msg_gateway* gw) {
    gateway = gw;

    // setup send to client flow
    ip_server.gateway = gateway;
    ip_server.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_server.next_dest_addr.set_next_choice(dest_ip);
    ip_server.next_source_addr.set_next_choice(src_ip);

    icmp_server.gateway = &ip_server;
    icmp_server.next_type.set_next_choice(ICMP_CONN_SERVER_TYPE);
    icmp_server.next_code.set_next_choice(ICMP_CONN_SERVER_CODE);

    // client app_handler would automatically use _udp_server.


    // setup recv from client flow
    gateway->add_listener(&ip_server);

    ip_server.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_server);

    icmp_server.type_handlers.assign_to_key(ICMP_CONN_CLIENT_TYPE, this);
}

int icmp_connection_server::send_data(send_msg<> &msg) {
    return icmp_server.send_data(msg);
}
