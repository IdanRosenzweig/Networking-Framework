#include "emp_server.h"

emp_server::emp_server(const udata_t& endpoint, ip4_addr src_ip, gateway* gw) : server_endpoint(endpoint), network_layer_gw(gw) {
    // setup send flow
    prot_stack.ip_prot.send_medium.add_send_channel(network_layer_gw);
    prot_stack.ip_prot.next_protocol.set_next_choice(0xa0);
    prot_stack.ip_prot.next_source_addr.set_next_choice(src_ip);

    prot_stack.emp_prot.send_medium.add_send_channel(&prot_stack.ip_prot);
    prot_stack.emp_prot.next_source_point.set_next_choice(server_endpoint);

    // setup recv flow
    network_layer_gw->add_listener(&prot_stack.ip_prot);

    prot_stack.ip_prot.listeners.append_new_empty_handler(&prot_stack.emp_prot);
    prot_stack.ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(0xa0);

    prot_stack.emp_prot.endpoints_handlers.add_word(endpoint)->key.push_back(this);
}

emp_server::~emp_server() {
    network_layer_gw->remove_listener(&prot_stack.ip_prot);
}

void emp_server::handle_callback(recv_msg_t &&data) {
    emp_packet_stack pack_stack;

    struct ip4_header ip_header;
    extract_from_network_order(&ip_header, data.buff_of_prot_from_end(1));
    pack_stack.source_addr = ip_header.source_addr;
    pack_stack.dest_addr = ip_header.dest_addr;

    uint8_t * curr = data.buff_of_prot_from_end(0);
    curr += decode_data(&pack_stack.source_point, curr);
    curr += decode_data(&pack_stack.dest_point, curr);

    pack_stack.plain_data = recv_msg_t(data.buff_at_curr_offset(), data.buff_cnt_at_curr_offset());

    this->recv_forwarder::handle_callback(std::move(pack_stack));
}

int emp_server::send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg_t&& msg) {
    prot_stack.ip_prot.next_dest_addr.set_next_choice(client_addr);
    prot_stack.emp_prot.next_dest_point.set_next_choice(dest_endpoint);

    return prot_stack.emp_prot.send_data(std::move(msg));
}
