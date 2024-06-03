#ifndef NETWORKING_VPN_CLIENT_H
#define NETWORKING_VPN_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../direct_connections/tcp/tcp_client.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"

#include "common.h"

class vpn_client : public gateway {
    tcp_client tcp;
    msg_boundary_seperator<> client;

public:
    vpn_client(ip4_addr daemon_ip, const string& iface);

    int send_data(send_msg_t&& data) override;
};


#endif //NETWORKING_VPN_CLIENT_H
