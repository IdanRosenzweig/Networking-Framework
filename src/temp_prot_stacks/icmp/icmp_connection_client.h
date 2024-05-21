#ifndef SERVERCLIENT_ICMP_CONNECTION_CLIENT_H
#define SERVERCLIENT_ICMP_CONNECTION_CLIENT_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../abstract/connection/msg_connection.h"
#include "../../abstract/gateway/msg_gateway.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "common.h"

class icmp_connection_client : public msg_connection {
public:
    msg_gateway * gateway;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    icmp_connection_client(ip4_addr dest_ip, ip4_addr src_ip, msg_gateway * network_layer_gw);

    int send_data(send_msg<>& msg) override;

};

#endif //SERVERCLIENT_ICMP_CONNECTION_CLIENT_H
