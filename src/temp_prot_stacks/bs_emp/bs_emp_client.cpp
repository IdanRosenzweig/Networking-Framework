#include "bs_emp_client.h"
#include "../../linux/hardware.h"

bs_emp_client::bs_emp_client(ip4_addr dest_ip, const udata_t& dest_endpoint, const udata_t& src_endpoint,
                             msg_gateway * gw) {
    if (gw == nullptr) {
        gateway = new network_layer_gateway("enp0s3");
    } else
        gateway = gw;

    // setup send flow
    ip_client.gateway = gateway;
    ip_client.next_protocol.set_next_choice(0xa0);
    ip_client.next_dest_addr.set_next_choice(dest_ip);
    ip_client.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

    emp_client.gateway = &ip_client;
    emp_client.next_source_point.set_next_choice(src_endpoint);
    emp_client.next_dest_point.set_next_choice(dest_endpoint);

    // setup recv flow
    gateway->add_listener(&ip_client);

    // todo change to both udp_client_server and tcpSession matching
    ip_client.protocol_handlers.assign_to_key(0xa0, &emp_client);

    emp_client.endpoints_handlers.add_word(src_endpoint)->key = this;
}

int bs_emp_client::send_data(send_msg<> &msg) {
    return emp_client.send_data(msg);
}
