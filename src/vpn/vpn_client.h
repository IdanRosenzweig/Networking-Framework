#ifndef SERVERCLIENT_VPN_CLIENT_H
#define SERVERCLIENT_VPN_CLIENT_H

#include "../temp_connections/tcp/tcp_client.h"
#include "../protocols/msg_boundary/msg_boundary_seperator.h"

#include "common.h"

class vpn_client : public basic_gateway {
    tcp_client tcp;
    msg_boundary_seperator<> client;

public:
    vpn_client(const string &vpn_daemon_ip) :
            tcp(vpn_daemon_ip, 5678, 1212), client(&tcp) {
        client.add_listener(this);
//tcp.add_listener(this);
    }

    int send_data(send_msg val) override {
        return client.send_data(val);
//        return tcp.send_data(val);
    }
};


#endif //SERVERCLIENT_VPN_CLIENT_H
