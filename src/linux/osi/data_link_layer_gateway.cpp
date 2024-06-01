#include "data_link_layer_gateway.h"
#include <iostream>
using namespace std;

data_link_layer_gateway::data_link_layer_gateway(const weak_ptr<iface_access_point>& access) : iface_access(access), if_gateway(access) {
    if_gateway.add_listener(this);
    this->send_medium.add_send_channel(&if_gateway);
}

data_link_layer_gateway::~data_link_layer_gateway() {
    if_gateway.remove_listener(this);
}

int data_link_layer_gateway::send_data(send_msg_t &&data) {
    return send_medium.send_data(std::move(data));
}

void data_link_layer_gateway::handle_callback(recv_msg_t &&data) {
    recv_forwarder::handle_callback(std::move(data));
}
