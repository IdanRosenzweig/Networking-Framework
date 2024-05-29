#ifndef NETWORKING_UDP_CLIENT_H
#define NETWORKING_UDP_CLIENT_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/gateway/gateway.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"

class udp_client : public connection {
public:
    gateway * network_layer_gw;
    ip4_protocol ip_prot;
    udp_protocol udp_prot;

    udp_client(ip4_addr dest_ip, uint16_t dest_port, uint16_t my_port, ip4_addr src_ip, gateway * gw);

    ~udp_client();

    int send_data(send_msg<>&& msg) override;

};


#endif //NETWORKING_UDP_CLIENT_H
