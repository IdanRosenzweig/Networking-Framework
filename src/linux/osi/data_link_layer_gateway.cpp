#include "data_link_layer_gateway.h"

data_link_layer_gateway::data_link_layer_gateway(const string &interface) : if_gateway(interface) {
    if_gateway.add_listener(this);
}

int data_link_layer_gateway::send_data(send_msg<>& val) {
    return if_gateway.send_data(val);
}
