#include "udp_server.h"
#include <linux/ip.h>
#include "../../linux/hardware.h"

udp_server::udp_server(int serverPort) : server_port(serverPort), gateway("enp0s3") {

    // setup send to tcpSession flow
    ip_server.gateway = &gateway;
    ip_server.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

    _udp_server.gateway = &ip_server;
    _udp_server.next_source_port.set_next_choice(server_port);


    // setup recv from tcpSession flow
    gateway.add_listener(&ip_server);

    ip_server.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_server);

    _udp_server.port_handlers.assign_to_key(server_port, this);
}

void udp_server::handle_received_event(received_msg &event) {
    socket_msg sock_msg;
    sock_msg.msg = event;
    sock_msg.source_port =
            ntohs( ((struct udp_header*) (event.data.get() + event.protocol_offsets.rbegin()->first))->source_port );
    sock_msg.dest_port =
            ntohs( ((struct udp_header*) (event.data.get() + event.protocol_offsets.rbegin()->first))->dest_port );
    extract_from_network_order(&sock_msg.source_addr,
                               (uint8_t*) &((struct iphdr*) (event.data.get() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);

    this->multi_receiver::handle_received_event(sock_msg);
}

int udp_server::send_data_to_client(ip4_addr client_addr, int dest_port, send_msg msg) {
    ip_server.next_dest_addr.set_next_choice(client_addr);
    _udp_server.next_dest_port.set_next_choice(dest_port);

    return _udp_server.send_data(msg);
}
