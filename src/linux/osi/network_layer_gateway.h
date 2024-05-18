#ifndef SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
#define SERVERCLIENT_NETWORK_LAYER_GATEWAY_H

#include "../../abstract/gateway/msg_gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../interface_gateway.h"

class network_layer_gateway : public msg_gateway {
private:
    // in typical local networks, the gateway to the network layer is through the router, by marking
    // ip protocol in the ethertype field

    interface_gateway interfaceGateway;
    ethernet_protocol ether_prot;

public:
    network_layer_gateway(const string& interface);

    int send_data(send_msg<>& msg) override;

};


#endif //SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
