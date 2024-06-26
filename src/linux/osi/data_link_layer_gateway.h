#ifndef NETWORKING_DATA_LINK_LAYER_GATEWAY_H
#define NETWORKING_DATA_LINK_LAYER_GATEWAY_H

#include "../../abstract/gateway/gateway.h"
#include "../../abstract/sending/send_forwarder.h"
#include "../if/wrappers/interface_gateway.h"
#include <memory>

class data_link_layer_gateway : public gateway {
    interface_gateway if_gateway;
    send_forwarder<send_msg_t> send_medium;

public:
    weak_ptr<iface_access_point> iface_access;

    data_link_layer_gateway(const weak_ptr<iface_access_point>& access);

    ~data_link_layer_gateway();

    int send_data(send_msg_t &&data) override;

    void handle_callback(recv_msg_t &&data) override;
};

#endif //NETWORKING_DATA_LINK_LAYER_GATEWAY_H
