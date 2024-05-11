#ifndef SERVERCLIENT_VPN_DAEMON_H
#define SERVERCLIENT_VPN_DAEMON_H

#include "../../abstract/connection/conn_aggregator.h"
#include "../../temp_connections/udp_client_server/udp_server.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"
#include "common.h"
#include "../../temp_connections/tcp_client_server/tcp_server.h"

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
//            master->aggregator.add_connection(master->tcpSession.back().get());
        }

    };
    sessions_handler handler;

    data_link_layer_gateway dataLinkLayerGateway;
    vector<std::unique_ptr<tcp_session>> tcpSession;
    vector<msg_boundary_seperator<>> sessions;

    conn_aggregator aggregator;

public:
    vpn_daemon(const string& local_interface) : dataLinkLayerGateway(local_interface), handler(this) {
        aggregator.add_connection(&dataLinkLayerGateway);
    }


};


#endif //SERVERCLIENT_VPN_DAEMON_H
