#ifndef SERVERCLIENT_ICMP_CONNECTION_CLIENT_H
#define SERVERCLIENT_ICMP_CONNECTION_CLIENT_H

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../linux/network_layer_gateway/network_layer_gateway.h"
#include "../../abstract/basic_connection.h"

class icmp_connection_client : public basic_connection {
public:
    network_layer_gateway gateway;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    icmp_connection_client() {
        system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_all");

        // setup send flow
        ip_client.gateway = &gateway;
        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr("10.100.102.18"));

        icmp_client.gateway = &ip_client;

        // setup recv flow
        gateway.add_listener(&ip_client);

        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

        icmp_client.type_handlers.assign_to_key(81, this);
    }

    int send_data(send_msg msg) override {
        return icmp_client.send_data(msg);
    }

};

#endif //SERVERCLIENT_ICMP_CONNECTION_CLIENT_H
