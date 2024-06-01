#ifndef NETWORKING_NETWORK_LAYER_GATEWAY_H
#define NETWORKING_NETWORK_LAYER_GATEWAY_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../tools/net_arp/net_arp.h"
#include "data_link_layer_gateway.h"
#include "../if/wrappers/interface_gateway.h"

class network_layer_gateway : public gateway {
private:
    // in typical local networks, the send_medium to the network layer is through the router,
    // as a default send_medium

    data_link_layer_gateway dataLinkLayerGateway;
    ethernet_protocol ether_prot;

    net_arp arp_handler;

public:
    network_layer_gateway(const weak_ptr<iface_access_point>& access);

    ~network_layer_gateway();

    int send_data(send_msg_t&&data) override;

    void handle_callback(recv_msg_t &&data) override;
};


#endif //NETWORKING_NETWORK_LAYER_GATEWAY_H
