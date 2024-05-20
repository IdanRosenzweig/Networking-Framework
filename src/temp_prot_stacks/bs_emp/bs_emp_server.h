#ifndef SERVERCLIENT_BS_EMP_SERVER_H
#define SERVERCLIENT_BS_EMP_SERVER_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/better_suite/bs_emp/bs_emp.h"
#include "../../abstract/connection/msg_connection.h"

#include "../../protocols/socket/socket_msg.h"

struct emp_packet_stack {
    received_msg msg;
    ip4_addr source_addr;
    udata_t source_point;
    udata_t dest_point;
};

class bs_emp_server : private msg_receiver, public multi_receiver<emp_packet_stack> {
    void handle_received_event(received_msg& event) override;

public:
    network_layer_gateway gateway;
    ip4_protocol ip_server;
    bs_emp emp_server;

    udata_t server_endpoint;

    bs_emp_server(const udata_t& endpoint);

    int send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg<> msg);

};

#endif //SERVERCLIENT_BS_EMP_SERVER_H
