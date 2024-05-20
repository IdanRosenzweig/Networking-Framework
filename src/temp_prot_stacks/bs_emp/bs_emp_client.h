#ifndef SERVERCLIENT_BS_EMP_CLIENT_H
#define SERVERCLIENT_BS_EMP_CLIENT_H

#include "../../abstract/connection/msg_connection.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/better_suite/bs_emp/bs_emp.h"

class bs_emp_client : public msg_connection {
public:
    msg_gateway * gateway;
    ip4_protocol ip_client;
    bs_emp emp_client;

    bs_emp_client(ip4_addr dest_ip, const udata_t& dest_endpoint, const udata_t& src_endpoint, msg_gateway * gw = nullptr);

    int send_data(send_msg<>& msg) override;

};


#endif //SERVERCLIENT_BS_EMP_CLIENT_H
