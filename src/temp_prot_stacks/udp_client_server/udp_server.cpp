#include "udp_server.h"
#include <linux/ip.h>
#include "../../linux/if/hardware.h"

udp_server::udp_server(uint16_t serverPort, ip4_addr src_ip, gateway* gw) : server_port(serverPort), network_layer_gw(gw) {

    // setup send flow
    ip_prot.send_medium.add_send_channel(network_layer_gw);
    ip_prot.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    udp_prot.send_medium.add_send_channel(&ip_prot);
    udp_prot.next_source_port.set_next_choice(server_port);


    // setup recv flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.listeners.append_new_empty_handler(&udp_prot);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_UDP);

    udp_prot.listeners.append_new_empty_handler(this);
    udp_prot.listeners.add_requirement_to_last_handler<UDP_LISTEN_ON_PORT_INDEX>(server_port);
}

udp_server::~udp_server() {
    network_layer_gw->remove_listener(&ip_prot);
}

void udp_server::handle_received_event(received_msg &&event) {
//    cout << "udp server received. forwarding" << endl;
    udp_packet_stack pack_stack;
    pack_stack.msg = std::move(event);
    pack_stack.source_port =
            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->source_port );
    pack_stack.dest_port =
            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->dest_port );
    extract_from_network_order(&pack_stack.source_addr,
                               (uint8_t*) &((struct iphdr*) (event.data.data() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);

    this->recv_forwarder<udp_packet_stack>::handle_received_event(std::move(pack_stack));
}

int udp_server::send_data_to_client(ip4_addr dest_addr, uint16_t dest_port, send_msg<> msg) {
    ip_prot.next_dest_addr.set_next_choice(dest_addr);
    udp_prot.next_dest_port.set_next_choice(dest_port);

    return udp_prot.send_data(std::move(msg));
}

