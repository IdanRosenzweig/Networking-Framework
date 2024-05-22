#ifndef SERVERCLIENT_ICMP_CONNECTION_SERVER_H
#define SERVERCLIENT_ICMP_CONNECTION_SERVER_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../linux/hardware.h"
#include "common.h"

// todo packet stack like in udp server
class icmp_connection_server : public connection {
public:
    gateway *network_layer_gw;
    ip4_protocol ip_server;
    icmp_protocol icmp_server;

    icmp_connection_server(ip4_addr dest_ip, ip4_addr src_ip, gateway* gw);

    int send_data(send_msg<>&& msg) override;
};

#endif //SERVERCLIENT_ICMP_CONNECTION_SERVER_H
