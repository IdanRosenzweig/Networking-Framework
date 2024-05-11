#ifndef SERVERCLIENT_PING_UTIL_H
#define SERVERCLIENT_PING_UTIL_H

#include "../../linux/network_layer_gateway/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"
#include "../../abstract/utils/circular_buffer.h"
#include <thread>

class ping_util : msg_receiver {
    network_layer_gateway gateway;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    circular_buffer<received_msg> raw_icmp;
    void handle_received_event(received_msg &event) override {
        raw_icmp.push_back(event);
    }

public:
    ping_util() : gateway("enp0s3") {
        // setup send flow
        ip_client.gateway = &gateway;
        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
//        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
        ip_client.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

        icmp_client.gateway = &ip_client;

        // setup recv flow
        gateway.add_listener(&ip_client);
        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

        icmp_client.default_handler = this;
    }

    next_choice<ip4_addr> dest_ip;
    next_choice<int> count;
    next_choice<std::chrono::duration<uint64_t, milli>> delay_interval;

    void ping_node();

};


#endif //SERVERCLIENT_PING_UTIL_H
