#ifndef SERVERCLIENT_EMP_SERVER_H
#define SERVERCLIENT_EMP_SERVER_H

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/emp/emp.h"
#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"

#include "../../protocols/socket/socket_msg.h"

struct emp_packet_stack {
    received_msg msg;
    ip4_addr source_addr;
    udata_t source_point;
    udata_t dest_point;
};

class emp_server : public msg_receiver, public receive_multiplexer<emp_packet_stack> {
    void handle_received_event(received_msg&& event) override;

public:
    gateway *network_layer_gw;
    ip4_protocol ip_prot;
    emp emp_prot;

    udata_t server_endpoint;

    emp_server(const udata_t& endpoint, ip4_addr src_ip, gateway* gw);

    int send_data_to_client(ip4_addr client_addr, const udata_t& dest_endpoint, send_msg<> msg);

};

#endif //SERVERCLIENT_EMP_SERVER_H
