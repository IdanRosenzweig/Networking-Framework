#ifndef SERVERCLIENT_NETWORK_LAYER_GATEWAY_CLIENT_H
#define SERVERCLIENT_NETWORK_LAYER_GATEWAY_CLIENT_H

#include "../../abstract/basic_gateway.h"
#include "../../protocols/ether/ethernet_conn_client.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../hardware.h"
#include <linux/if_ether.h>

class network_layer_gateway_client : public basic_gateway {
private:
    // in typical local networks, the gateway to the network layer is through the router, by marking
    // ip protocol in the ethertype field

    data_link_layer_gateway data_link_gateway;
    ethernet_conn_client ether_client;

public:
    network_layer_gateway_client() {
        // send
        ether_client.gateway = &data_link_gateway;
        ether_client.next_protocol.set_next_choice(htons(ETH_P_IP));
        ether_client.next_mac_addr.set_next_choice(
                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
        );
        ether_client.my_addr.set_next_choice(get_my_mac_address("enp0s3"));

        // recv
        data_link_gateway.add_listener(&ether_client);
        ether_client.prot_handlers[htons(ETH_P_IP)] = this;
    }

    int send_data(void *buff, int count) override;

};


#endif //SERVERCLIENT_NETWORK_LAYER_GATEWAY_CLIENT_H
