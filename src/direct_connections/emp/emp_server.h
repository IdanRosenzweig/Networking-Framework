#ifndef NETWORKING_EMP_SERVER_H
#define NETWORKING_EMP_SERVER_H

#include "../../abstract/gateway/gateway.h"
#include "../../protocol_stacks/emp_stack.h"



class emp_server : public msg_recv_listener, public recv_forwarder<emp_packet_stack> {
    void handle_callback(recv_msg_t&& data) override;

public:
    gateway *network_layer_gw;
    emp_protocol_stack prot_stack;

    udata_t server_endpoint;

    emp_server(const udata_t& endpoint, ip4_addr src_ip, gateway* gw);

    ~emp_server();

    int send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg_t&& msg);

};

#endif //NETWORKING_EMP_SERVER_H
