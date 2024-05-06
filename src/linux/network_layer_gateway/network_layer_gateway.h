#ifndef SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
#define SERVERCLIENT_NETWORK_LAYER_GATEWAY_H

#include "../../abstract/connection/basic_gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../hardware.h"

class network_layer_gateway : public basic_gateway {
private:
    // in typical local networks, the gateway to the network layer is through the router, by marking
    // ip protocol in the ethertype field

    data_link_layer_gateway data_link_gateway;
    ethernet_protocol ether_prot;

public:
    network_layer_gateway(const string& interface);

    int send_data(send_msg msg) override;

};


#endif //SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
