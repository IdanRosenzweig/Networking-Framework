#include "network_layer_gateway.h"
#include <linux/if_ether.h>

network_layer_gateway::network_layer_gateway(const string& interface) : data_link_gateway(interface) {
    // send
    ether_prot.gateway = &data_link_gateway;
    ether_prot.next_protocol.set_next_choice(htons(ETH_P_IP));
    ether_prot.next_dest_addr.set_next_choice(
            {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
    );
    ether_prot.next_source_addr.set_next_choice(get_my_mac_address("enp0s3"));

    // recv
    data_link_gateway.add_listener(&ether_prot);
    ether_prot.protocol_handlers.assign_to_key(htons(ETH_P_IP), this);
}

int network_layer_gateway::send_data(send_msg msg) {
    return ether_prot.send_data({msg.buff, msg.count});
}


