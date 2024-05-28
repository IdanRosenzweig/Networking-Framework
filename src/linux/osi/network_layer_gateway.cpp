#include "network_layer_gateway.h"
#include <linux/if_ether.h>
#include "../if/hardware.h"
#include <iostream>

using namespace std;

network_layer_gateway::network_layer_gateway(const weak_ptr<iface_access_point>& access) : dataLinkLayerGateway(access), arp_handler(&dataLinkLayerGateway) {

    mac_addr my_mac;
    ip4_addr my_ip;
    ip4_addr default_gw;
    if (shared_ptr<iface_access_point> ptr = dataLinkLayerGateway.iface_access.lock()) {
        my_mac = get_mac_addr_of_iface(ptr->iface_name);
        my_ip = get_ip_addr_of_iface(ptr->iface_name);
        default_gw = get_default_gateway_of_iface(ptr->iface_name);
    } else return;
//    std::cout << "default send_medium is " << convert_to_str(default_gw) << endl;

    // send
    ether_prot.send_medium = &dataLinkLayerGateway;
    ether_prot.next_protocol.set_next_choice(htons(ETH_P_IP));
    ether_prot.next_dest_addr.set_next_choice(
            arp_handler.search_for_mac_addr(default_gw, my_mac, my_ip)
    );
    ether_prot.next_source_addr.set_next_choice(my_mac);

    // recv
    dataLinkLayerGateway.add_listener(&ether_prot);
    ether_prot.protocol_handlers[htons(ETH_P_IP)].push_back( this);
}

network_layer_gateway::~network_layer_gateway() {
    dataLinkLayerGateway.remove_listener(&ether_prot);
}

int network_layer_gateway::send_data(send_msg<> &&msg) {
    return ether_prot.send_data(std::move(msg));
}

void network_layer_gateway::handle_received_event(received_msg &&event) {
    recv_forwarder::handle_received_event(std::move(event));
}


