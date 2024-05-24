#include "emp_server.h"
#include <linux/ip.h>
#include "../../linux/hardware.h"

emp_server::emp_server(const udata_t& endpoint, ip4_addr src_ip, gateway* gw) : server_endpoint(endpoint), network_layer_gw(gw) {

    // setup send to raw_session flow
    ip_prot.gateway = network_layer_gw;
    ip_prot.next_protocol.set_next_choice(0xa0);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    emp_prot.gateway = &ip_prot;
    emp_prot.next_source_point.set_next_choice(server_endpoint);


    // setup recv from raw_session flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.protocol_handlers.assign_to_key(0xa0, &emp_prot);

    emp_prot.endpoints_handlers.add_word(endpoint)->key = this;
}

emp_server::~emp_server() {
    network_layer_gw->remove_listener(&ip_prot);
}

void emp_server::handle_received_event(received_msg &&event) {
    emp_packet_stack pack_stack;
    pack_stack.msg = event;

    uint8_t * curr = event.data.data() + event.protocol_offsets.rbegin()->first;
    curr += decode_data(&pack_stack.source_point, curr);
    curr += decode_data(&pack_stack.dest_point, curr);

    extract_from_network_order(&pack_stack.source_addr,
                               (uint8_t*) &((struct iphdr*) (event.data.data() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);

    this->receive_multiplexer::handle_received_event(std::move(pack_stack));
}

int emp_server::send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg<> msg) {
    ip_prot.next_dest_addr.set_next_choice(client_addr);
    emp_prot.next_dest_point.set_next_choice(dest_endpoint);

    return emp_prot.send_data(std::move(msg));
}
