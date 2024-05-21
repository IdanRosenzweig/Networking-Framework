#ifndef SERVERCLIENT_EMP_CLIENT_H
#define SERVERCLIENT_EMP_CLIENT_H

#include "../../abstract/connection/msg_connection.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/emp/emp.h"

class emp_client : public msg_connection {
public:
    msg_gateway * gateway;
    ip4_protocol ip_prot;
    emp emp_prot;

    emp_client(ip4_addr dest_ip, const udata_t& dest_endpoint, const udata_t& src_endpoint, ip4_addr src_ip, msg_gateway * network_layer_gw);

    int send_data(send_msg<>& msg) override;

};


#endif //SERVERCLIENT_EMP_CLIENT_H
