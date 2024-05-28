#ifndef SERVERCLIENT_BLOCK_INCOMING_UDP_H
#define SERVERCLIENT_BLOCK_INCOMING_UDP_H

#include <netinet/in.h>
#include <linux/if_ether.h>

#include "../../abstract/firewall/basic_firewall_filter.h"
#include "../../abstract/utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

class block_incoming_udp : public basic_firewall_filter<received_msg> {
    ip4_protocol ip_prot;
    udp_protocol udp_prot;

    class data_counter counter;

public:
    block_incoming_udp(int port) {
        ip_prot.protocol_handlers[IPPROTO_UDP].push_back( &udp_prot);
        udp_prot.port_handlers[port].push_back(&counter);
    }

    firewall_policy calc_policy(const received_msg& msg) override {
        received_msg copy = msg;
        ip_prot.handle_received_event(std::move(copy));

        int cnt = counter.get_count();
        counter.reset();

        if (cnt == 1) return BLOCK;
        else return ALLOW;
    }
};

#endif //SERVERCLIENT_BLOCK_INCOMING_UDP_H
