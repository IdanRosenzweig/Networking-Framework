#include "vpn_client.h"

vpn_client::vpn_client(ip4_addr daemon_ip, const string& iface) :
        tcp(daemon_ip, VPN_DAEMON_PORT, VPN_CLIENT_PORT, iface), client(&tcp) {
    client.add_listener(this);
//tcp.add_listener(this);
}

int vpn_client::send_data(send_msg_t &&data) {
    return client.send_data(std::move(data));
//        return tcp.send_data(val);
}
