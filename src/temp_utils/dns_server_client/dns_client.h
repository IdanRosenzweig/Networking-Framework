#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/received_msg.h"
#include "../../abstract/sending/msg_sender.h"

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"

#include "dns.h"

class dns_client : public recv_queue<received_msg> {
public:

//    osi networkLayerGateway;
    basic_gateway* gateway;
    ip4_protocol ip_client;
    udp_protocol udp_client;

    dns_client(const ip4_addr server_addr, basic_gateway* gw = nullptr) {
        if (gw == nullptr) gateway = new network_layer_gateway("enp0s3");
        else gateway = gw;

        // setup send flow
        ip_client.gateway = gateway;
        ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
        ip_client.next_dest_addr.set_next_choice(server_addr);
        ip_client.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));
//        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));

        udp_client.gateway = &ip_client;
        udp_client.next_source_port.set_next_choice(1212);
        udp_client.next_dest_port.set_next_choice(DNS_SERVER_PORT);

        // setup recv flow
        gateway->add_listener(&ip_client);

        ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

        udp_client.port_handlers.assign_to_key(1212, this);
    }

    void query(dns_record_type type, const std::string& key);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
