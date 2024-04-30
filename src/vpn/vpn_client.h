#ifndef SERVERCLIENT_VPN_CLIENT_H
#define SERVERCLIENT_VPN_CLIENT_H

#include "../temp_connections/udp/udp_client.h"

#include "common.h"

class vpn_client : public basic_gateway {
    udp_client udp;


public:
    vpn_client(const string &vpn_daemon_ip) :
            udp(vpn_daemon_ip, VPN_DAEMON_PORT, VPN_CLIENT_PORT) {
        udp.add_listener(this);
    }

    int send_data(send_msg val) override {
        return udp.send_data(val);
    }
};


#endif //SERVERCLIENT_VPN_CLIENT_H
