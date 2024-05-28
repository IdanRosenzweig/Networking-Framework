#ifndef NETWORKING_ICMP_CONNECTION_CLIENT_H
#define NETWORKING_ICMP_CONNECTION_CLIENT_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"
#include "../../linux/if/hardware.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "common.h"

class icmp_connection_client : public connection {
public:
    gateway * network_layer_gw;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    icmp_connection_client(ip4_addr dest_ip, ip4_addr src_ip, gateway * gw);

    ~icmp_connection_client();

    int send_data(send_msg<>&& msg) override;

};

#endif //NETWORKING_ICMP_CONNECTION_CLIENT_H
