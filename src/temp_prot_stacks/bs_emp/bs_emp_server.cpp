#include "bs_emp_server.h"
#include <linux/ip.h>
#include "../../linux/hardware.h"

bs_emp_server::bs_emp_server(const udata_t& endpoint) : server_endpoint(endpoint), gateway("enp0s3") {

    // setup send to tcpSession flow
    ip_server.gateway = &gateway;
    ip_server.next_protocol.set_next_choice(0xa0);
    ip_server.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

    emp_server.gateway = &ip_server;
    emp_server.next_source_point.set_next_choice(server_endpoint);


    // setup recv from tcpSession flow
    gateway.add_listener(&ip_server);

    ip_server.protocol_handlers.assign_to_key(0xa0, &emp_server);

    emp_server.endpoints_handlers.add_word(endpoint)->key = this;
}

void bs_emp_server::handle_received_event(received_msg &event) {
    emp_packet_stack pack_stack;
    pack_stack.msg = event;

    uint8_t * curr = event.data.data() + event.protocol_offsets.rbegin()->first;
    curr += decode_data(&pack_stack.source_point, curr);
    curr += decode_data(&pack_stack.dest_point, curr);

    extract_from_network_order(&pack_stack.source_addr,
                               (uint8_t*) &((struct iphdr*) (event.data.data() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);

    this->multi_receiver::handle_received_event(pack_stack);
}

int bs_emp_server::send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg<> msg) {
    ip_server.next_dest_addr.set_next_choice(client_addr);
    emp_server.next_dest_point.set_next_choice(dest_endpoint);

    return emp_server.send_data(msg);
}
