#ifndef SERVERCLIENT_ICMP_CONNECTION_SERVER_H
#define SERVERCLIENT_ICMP_CONNECTION_SERVER_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../linux/network_layer_gateway/network_layer_gateway.h"
#include "../../abstract/connection/basic_connection.h"
#include "common.h"

class icmp_connection_server : public basic_connection {
public:
    network_layer_gateway gateway;
    ip4_protocol ip_server;
    icmp_protocol icmp_server;

    icmp_connection_server(const string& dest_ip) : gateway("enp0s3") {

        // setup send to client flow
        ip_server.gateway = &gateway;
        ip_server.next_protocol.set_next_choice(IPPROTO_ICMP);
        ip_server.next_dest_addr.set_next_choice(convert_to_ip4_addr(dest_ip));
        ip_server.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

        icmp_server.gateway = &ip_server;
        icmp_server.next_type.set_next_choice(ICMP_CONN_SERVER_TYPE);
        icmp_server.next_code.set_next_choice(ICMP_CONN_SERVER_CODE);

        // client handler would automatically use _udp_server.


        // setup recv from client flow
        gateway.add_listener(&ip_server);

        ip_server.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_server);

        icmp_server.type_handlers.assign_to_key(ICMP_CONN_CLIENT_TYPE, this);
    }

    int send_data(send_msg msg) override {
        return icmp_server.send_data(msg);
    }
};

#endif //SERVERCLIENT_ICMP_CONNECTION_SERVER_H
