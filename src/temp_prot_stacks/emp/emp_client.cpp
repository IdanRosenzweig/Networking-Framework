#include "emp_client.h"
#include "../../linux/hardware.h"

emp_client::emp_client(ip4_addr dest_ip, const udata_t& dest_endpoint, const udata_t& src_endpoint,
                       ip4_addr src_ip, gateway * gw) : network_layer_gw(gw) {
    // setup send flow
    ip_prot.gateway = network_layer_gw;
    ip_prot.next_protocol.set_next_choice(0xa0);
    ip_prot.next_dest_addr.set_next_choice(dest_ip);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    emp_prot.gateway = &ip_prot;
    emp_prot.next_source_point.set_next_choice(src_endpoint);
    emp_prot.next_dest_point.set_next_choice(dest_endpoint);

    // setup recv flow
    network_layer_gw->add_listener(&ip_prot);

    // todo change to both udp_client_server and raw_session matching
    ip_prot.protocol_handlers[0xa0].push_back( &emp_prot);

    emp_prot.endpoints_handlers.add_word(src_endpoint)->key.push_back(this);
}

emp_client::~emp_client() {
    network_layer_gw->remove_listener(&ip_prot);
}

int emp_client::send_data(send_msg<> &&msg) {
    return emp_prot.send_data(std::move(msg));
}
