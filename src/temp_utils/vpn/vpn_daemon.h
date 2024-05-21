#ifndef SERVERCLIENT_VPN_DAEMON_H
#define SERVERCLIENT_VPN_DAEMON_H

#include "../../abstract/connection/conn_aggregator.h"
#include "../../abstract/firewall/firewall.h"
#include "../../abstract/firewall/block_udp.h"
#include "../../abstract/firewall/block_tcp.h"
#include "../../temp_prot_stacks/udp_client_server/udp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_server.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include "common.h"

class vpn_daemon {

    class sessions_handler : public basic_receiver<std::unique_ptr<tcp_session>> {
        vpn_daemon* master;
        tcp_server tcpServer;
    public:

        explicit sessions_handler(vpn_daemon *master) : master(master), tcpServer(5678) {
            tcpServer.add_listener(this);
        }

        void handle_received_event(std::unique_ptr<tcp_session>& event) override {
            master->tcpSession.push_back(std::move(event));

            master->sessions.emplace_back(master->tcpSession.back().get());
            master->aggregator.add_connection(&master->sessions.back());
        }

    };
    sessions_handler handler;

    data_link_layer_gateway dataLinkLayerGateway;
    block_tcp_filter block_filter;
    firewall gateway_firewall;

    vector<std::unique_ptr<tcp_session>> tcpSession;
    vector<msg_boundary_seperator<>> sessions;

    conn_aggregator aggregator;

public:
    vpn_daemon(const string& local_interface) : dataLinkLayerGateway(local_interface)
    , gateway_firewall(&dataLinkLayerGateway), block_filter(5678)
    , handler(this)
                                                {
        gateway_firewall.incoming_filters.push_back(&block_filter);
        aggregator.add_connection(&gateway_firewall);
    }


};


#endif //SERVERCLIENT_VPN_DAEMON_H
