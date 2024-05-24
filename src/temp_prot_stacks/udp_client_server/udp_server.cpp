#include "udp_server.h"
#include <linux/ip.h>
#include "../../linux/hardware.h"

udp_server::udp_server(int serverPort, ip4_addr src_ip, gateway* gw) : server_port(serverPort), network_layer_gw(gw) {

    // setup send flow
    ip_server.gateway = network_layer_gw;
    ip_server.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_source_addr.set_next_choice(src_ip);

    _udp_server.gateway = &ip_server;
    _udp_server.next_source_port.set_next_choice(server_port);


    // setup recv flow
    network_layer_gw->add_listener(&ip_server);

    ip_server.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_server);

    _udp_server.port_handlers.assign_to_key(server_port, (basic_receiver<received_msg> *) this);
}

udp_server::~udp_server() {
    network_layer_gw->remove_listener(&ip_server);
}

void udp_server::handle_received_event(received_msg &&event) {
    udp_packet_stack pack_stack;
    pack_stack.msg = std::move(event);
    pack_stack.source_port =
            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->source_port );
    pack_stack.dest_port =
            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->dest_port );
    extract_from_network_order(&pack_stack.source_addr,
                               (uint8_t*) &((struct iphdr*) (event.data.data() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);

    this->receive_multiplexer<udp_packet_stack>::handle_received_event(std::move(pack_stack));
}

int udp_server::send_data_to_client(ip4_addr dest_addr, int dest_port, send_msg<> msg) {
    ip_server.next_dest_addr.set_next_choice(dest_addr);
    _udp_server.next_dest_port.set_next_choice(dest_port);

    return _udp_server.send_data(std::move(msg));
}

