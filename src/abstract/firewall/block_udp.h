#ifndef SERVERCLIENT_BLOCK_UDP_H
#define SERVERCLIENT_BLOCK_UDP_H

#include <netinet/in.h>
#include <linux/if_ether.h>

#include "basic_firewall_filter.h"
#include "../utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

class block_udp_filter : public basic_firewall_filter<received_msg> {
public:
    ip4_protocol ip_prot;
    udp_protocol udp_prot;

    class data_counter counter;

    block_udp_filter(int port) {
        ip_prot.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_prot);
        udp_prot.port_handlers.assign_to_key(port, &counter);
    }

    firewall_policy calc_policy(received_msg& msg) override {
        ip_prot.handle_received_event(msg);

        int cnt = counter.get_count();
        counter.reset();

        if (cnt == 1) return BLOCK;
        else return ALLOW;
    }
};

#endif //SERVERCLIENT_BLOCK_UDP_H
