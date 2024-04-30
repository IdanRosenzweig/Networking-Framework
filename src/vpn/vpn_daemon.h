#ifndef SERVERCLIENT_VPN_DAEMON_H
#define SERVERCLIENT_VPN_DAEMON_H

#include "../abstract/connection/basic_forwarder_daemon.h"
#include "../temp_connections/udp/udp_server.h"

#include "common.h"

class vpn_daemon {

    class server_handler : public basic_receiver<socket_msg>, public basic_connection {
    public:
        udp_server udpConnServer;

        server_handler() : udpConnServer(VPN_DAEMON_PORT) {
            udpConnServer.add_listener(this);
        }

        void handle_received_event(socket_msg& event) override {
            this->listenable::handle_received_event(event.msg);
        }

        string client_addr = "10.100.102.18";
        int send_data(send_msg val) override {
            return udpConnServer.send_data_to_client(convert_to_ip4_addr(client_addr), 1001, val);
        }
    };
    server_handler conn;

    data_link_layer_gateway dataLinkLayerGateway;

    basic_forwarder_daemon daemon;

public:
    vpn_daemon() : daemon(&conn, &dataLinkLayerGateway) {

    }
};


#endif //SERVERCLIENT_VPN_DAEMON_H
