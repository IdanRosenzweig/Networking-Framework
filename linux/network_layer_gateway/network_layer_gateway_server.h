#ifndef SERVERCLIENT_NETWORK_LAYER_GATEWAY_SERVER_H
#define SERVERCLIENT_NETWORK_LAYER_GATEWAY_SERVER_H

#include "../../abstract/basic_gateway.h"
#include "../../protocols/ether/ethernet_conn_server.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../hardware.h"
#include <linux/if_ether.h>

class network_layer_gateway_server : public basic_gateway {
private:
    // in typical local networks, the gateway to the network layer is through the router, by marking
    // ip protocol in the ethertype field

    data_link_layer_gateway data_link_gateway;
    ethernet_conn_server ether_server;

public:
    network_layer_gateway_server() {
        ether_server.prot_handlers[htons(ETH_P_IP)] = this;

        // send
        ether_server.gateway = &data_link_gateway;
        ether_server.next_protocol.set_next_choice(htons(ETH_P_IP));
        ether_server.next_dest_addr.set_next_choice(
                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
        );
        ether_server.my_addr.set_next_choice(get_my_mac_address("enp0s3"));

        // recv
        data_link_gateway.add_listener(&ether_server);
        ether_server.prot_handlers[htons(ETH_P_IP)] = this;
    }

    int send_data(void *buff, int count) override;

};


#endif //SERVERCLIENT_NETWORK_LAYER_GATEWAY_SERVER_H
