#ifndef SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
#define SERVERCLIENT_NETWORK_LAYER_GATEWAY_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../temp_utils/net_arp/net_arp.h"
#include "../interface_gateway.h"

class network_layer_gateway : public gateway {
private:
    // in typical local networks, the data_link_layer_gateway to the network layer is through the router, by marking
    // ip protocol in the ethertype field

    interface_gateway interfaceGateway;
    ethernet_protocol ether_prot;

    net_arp arp_handler;

public:
    network_layer_gateway(const string& interface);

    int send_data(send_msg<>&& msg) override;

};


#endif //SERVERCLIENT_NETWORK_LAYER_GATEWAY_H
