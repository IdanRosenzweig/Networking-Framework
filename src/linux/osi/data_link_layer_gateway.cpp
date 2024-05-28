#include "data_link_layer_gateway.h"
#include <iostream>
using namespace std;

data_link_layer_gateway::data_link_layer_gateway(const weak_ptr<iface_access_point>& access) : iface_access(access),
                                                                                               if_gateway(access)
//, if_gateway2(access)
{
    if_gateway.add_listener(this);
//    if_gateway2.add_listener(this);
    this->send_medium.add_send_medium(&if_gateway);
//    this->send_medium.add_send_medium(&if_gateway2);
}

data_link_layer_gateway::~data_link_layer_gateway() {
    if_gateway.remove_listener(this);
//    if_gateway2.remove_listener(this);
}

int data_link_layer_gateway::send_data(send_msg<> &&val) {
    return send_medium.send_data(std::move(val));
}

void data_link_layer_gateway::handle_received_event(received_msg &&event) {
    recv_forwarder::handle_received_event(std::move(event));
}
