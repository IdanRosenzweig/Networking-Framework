#include "icmp_client.h"

icmp_client::icmp_client(ip4_addr dest_ip, ip4_addr src_ip, gateway * gw) : network_layer_gw(gw) {
    // setup send flow
    prot_stack.ip_prot.send_medium.add_send_channel(network_layer_gw);
    prot_stack.ip_prot.next_protocol.set_next_choice(ip4_header::protocol_values::icmp);
    prot_stack.ip_prot.next_dest_addr.set_next_choice(dest_ip);
    prot_stack.ip_prot.next_source_addr.set_next_choice(src_ip);

    prot_stack.icmp_prot.send_medium.add_send_channel(&prot_stack.ip_prot);
    prot_stack.icmp_prot.next_type.set_next_choice(ICMP_CONN_CLIENT_TYPE);
    prot_stack.icmp_prot.next_code.set_next_choice(ICMP_CONN_CLIENT_CODE);

    // setup recv flow
    network_layer_gw->add_listener(&prot_stack.ip_prot);

    prot_stack.ip_prot.listeners.append_new_empty_handler(&prot_stack.icmp_prot);
    prot_stack.ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::icmp);

    prot_stack.icmp_prot.listeners.append_new_empty_handler(this);
    prot_stack.icmp_prot.listeners.add_requirement_to_last_handler<ICMP_LISTEN_ON_TYPE_INDEX>(ICMP_CONN_SERVER_TYPE);
}

icmp_client::~icmp_client() {
    network_layer_gw->remove_listener(&prot_stack.ip_prot);
}

int icmp_client::send_data(send_msg_t &&data) {
    return prot_stack.icmp_prot.send_data(std::move(data));
}

void icmp_client::handle_callback(recv_msg_t &&data) {
    icmp_packet_stack pack_stack;

    struct ip4_header ip_header;
    extract_from_network_order(&ip_header, data.buff_of_prot_from_end(1));
    pack_stack.source_addr = ip_header.source_addr;
    pack_stack.dest_addr = ip_header.dest_addr;

    struct icmp_header icmp_header;
    extract_from_network_order(&icmp_header,  data.buff_of_prot_from_end(0));
    pack_stack.type = icmp_header.type;
    pack_stack.code = icmp_header.code;

    pack_stack.plain_data = recv_msg_t(data.buff_at_curr_offset(), data.buff_cnt_at_curr_offset());

    this->recv_forwarder<icmp_packet_stack>::handle_callback(std::move(pack_stack));

    this->recv_forwarder<recv_msg_t>::handle_callback(std::move(data));
}
