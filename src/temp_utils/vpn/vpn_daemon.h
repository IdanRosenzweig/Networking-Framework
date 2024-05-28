#ifndef NETWORKING_VPN_DAEMON_H
#define NETWORKING_VPN_DAEMON_H

#include "../../abstract/connection/conn_aggregator.h"
#include "../../abstract/firewall/firewall.h"
#include "block_incoming_udp.h"
#include "block_incoming_tcp.h"
#include "../../temp_prot_stacks/udp_client_server/udp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_server.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include "common.h"

class vpn_daemon {
    tcp_server tcpServer;

    class sessions_handler : public basic_recv_listener<tcp_session_type> {
        vpn_daemon *master;
    public:

        explicit sessions_handler(vpn_daemon *_master) : master(_master) {
            master->tcpServer.set_generator_listener(this);
        }

        void handle_received_event(tcp_session_type &&event) override {
            master->tcpSession.push_back(std::move(event));

//            master->sessions.emplace_back(master->tcpSession.back().sess_conn.get());
//            master->aggregator.add_connection(&master->sessions.back());
            master->sessions.emplace_back(make_unique<msg_boundary_seperator<>>(master->tcpSession.back().sess_conn.get()));
            master->aggregator.add_connection(master->sessions.back().get());
        }

    } handler;

    data_link_layer_gateway dataLinkLayerGateway;
    block_incoming_tcp block_filter;
    firewall gateway_firewall;

    vector<tcp_session_type> tcpSession;
//    vector<msg_boundary_seperator<>> sessions;
    vector<unique_ptr<msg_boundary_seperator<>>> sessions;

    conn_aggregator aggregator;

public:
    vpn_daemon(const weak_ptr<iface_access_point>& access) : dataLinkLayerGateway(access),
                                                gateway_firewall(&dataLinkLayerGateway), block_filter(5678),
                                                tcpServer(5678), handler(this) {
        gateway_firewall.incoming_filters.push_back(&block_filter);
        aggregator.add_connection(&gateway_firewall);
    }


};


#endif //NETWORKING_VPN_DAEMON_H
