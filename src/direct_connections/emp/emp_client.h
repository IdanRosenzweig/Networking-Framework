#ifndef NETWORKING_EMP_CLIENT_H
#define NETWORKING_EMP_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/emp_stack.h"

class emp_client : public connection, public recv_forwarder<emp_packet_stack> {
public:
    gateway * network_layer_gw;
    emp_protocol_stack prot_stack;

    emp_client(ip4_addr dest_ip, const udata_t& dest_endpoint, const udata_t& src_endpoint, ip4_addr src_ip, gateway * gw);

    ~emp_client();

    int send_data(send_msg_t&&data) override;

    void handle_callback(recv_msg_t &&data) override;
};


#endif //NETWORKING_EMP_CLIENT_H
