#include "udp_server.h"

udp_server::udp_server(uint16_t serverPort, ip4_addr src_ip, gateway* gw) : server_port(serverPort), network_layer_gw(gw) {
    // setup send flow
    prot_stack.ip_prot.send_medium.add_send_channel(network_layer_gw);
    prot_stack.ip_prot.next_protocol.set_next_choice(ip4_header::protocol_values::udp);
    prot_stack.ip_prot.next_source_addr.set_next_choice(src_ip);

    prot_stack.udp_prot.send_medium.add_send_channel(&prot_stack.ip_prot);
    prot_stack.udp_prot.next_source_port.set_next_choice(server_port);


    // setup recv flow
    network_layer_gw->add_listener(&prot_stack.ip_prot);

    prot_stack.ip_prot.listeners.append_new_empty_handler(&prot_stack.udp_prot);
    prot_stack.ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::udp);

    prot_stack.udp_prot.listeners.append_new_empty_handler(this);
    prot_stack.udp_prot.listeners.add_requirement_to_last_handler<UDP_LISTEN_ON_DEST_PORT_INDEX>(server_port);
}

udp_server::~udp_server() {
    network_layer_gw->remove_listener(&prot_stack.ip_prot);
}

void udp_server::handle_callback(recv_msg_t &&data) {
    udp_packet_stack pack_stack;

    struct ip4_header ip_header;
    extract_from_network_order(&ip_header, data.buff_of_prot_from_end(1));
    pack_stack.source_addr = ip_header.source_addr;
    pack_stack.dest_addr = ip_header.dest_addr;

    struct udp_header udp_header;
    extract_from_network_order(&udp_header, data.buff_of_prot_from_end(0));
    pack_stack.source_port = udp_header.source_port;
    pack_stack.dest_port = udp_header.dest_port;

    pack_stack.plain_data = recv_msg_t(data.buff_at_curr_offset(), data.buff_cnt_at_curr_offset());

    this->recv_forwarder<udp_packet_stack>::handle_callback(std::move(pack_stack));
}

int udp_server::send_data_to_client(ip4_addr dest_addr, uint16_t dest_port, send_msg_t&& msg) {
    prot_stack.ip_prot.next_dest_addr.set_next_choice(dest_addr);
    prot_stack.udp_prot.next_dest_port.set_next_choice(dest_port);

    return prot_stack.udp_prot.send_data(std::move(msg));
}

