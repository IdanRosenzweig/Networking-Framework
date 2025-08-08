#ifndef NETWORKING_VPN_DAEMON_H
#define NETWORKING_VPN_DAEMON_H

#include "../../abstract/connection/conn_aggregator.h"
#include "../../abstract/firewall/firewall.h"
#include "block_incoming_udp.h"
#include "block_incoming_tcp.h"
#include "../../direct_connections/udp/udp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../direct_connections/tcp/tcp_server.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include "common.h"

class vpn_daemon {
    tcp_server tcpServer;

    class sessions_handler : public recv_listener<tcp_session_type> {
        vpn_daemon *master;
    public:

        explicit sessions_handler(vpn_daemon *_master);

        void handle_callback(tcp_session_type &&data) override;

    } handler;

    gateway * data_link_layer_gw;
    block_incoming_tcp block_filter;
    firewall gateway_firewall;

    vector<tcp_session_type> tcpSession;
//    vector<msg_boundary_seperator<>> sessions;
    vector<unique_ptr<msg_boundary_seperator<>>> sessions;

    conn_aggregator aggregator;

public:
    vpn_daemon(const string& iface, gateway *gw);


};


#endif //NETWORKING_VPN_DAEMON_H
