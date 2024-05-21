#ifndef SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
#define SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H

#include "../../abstract/gateway/msg_gateway.h"
#include "../interface_gateway.h"

class data_link_layer_gateway : public msg_gateway {
private:

    interface_gateway if_gateway;

public:
    data_link_layer_gateway(const string& interface);

    int send_data(send_msg<>& val) override;

    void handle_received_event(received_msg &event) override;
};

#endif //SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
