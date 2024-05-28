#include "network_layer_gateway.h"
#include <linux/if_ether.h>
#include "../hardware.h"
#include <iostream>

using namespace std;

network_layer_gateway::network_layer_gateway(const string &interface) : interfaceGateway(interface),
                                                                        arp_handler(&interfaceGateway) {

    mac_addr my_mac = get_mac_addr_of_iface(interface);
    ip4_addr my_ip = get_ip_addr_of_iface(interface);
    ip4_addr default_gw = get_default_gateway_of_iface(interface);
//    std::cout << "default gateway is " << convert_to_str(default_gw) << endl;

    // send
    ether_prot.gateway = &interfaceGateway;
    ether_prot.next_protocol.set_next_choice(htons(ETH_P_IP));
    ether_prot.next_dest_addr.set_next_choice(
            arp_handler.search_for_mac_addr(default_gw, my_mac, my_ip)
    );
    ether_prot.next_source_addr.set_next_choice(my_mac);

    // recv
    interfaceGateway.add_listener(&ether_prot);
    ether_prot.protocol_handlers[htons(ETH_P_IP)].push_back( this);
}

int network_layer_gateway::send_data(send_msg<> &&msg) {
    return ether_prot.send_data(std::move(msg));
}


