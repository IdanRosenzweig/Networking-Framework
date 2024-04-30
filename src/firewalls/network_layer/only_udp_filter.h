#ifndef SERVERCLIENT_ONLY_UDP_FILTER_H
#define SERVERCLIENT_ONLY_UDP_FILTER_H

#include <netinet/in.h>
#include <linux/if_ether.h>

#include "../../abstract/firewall/basic_firewall_filter.h"
#include "../../abstract/firewall/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

template <int PORT>
class only_udp_filter : public basic_firewall_filter {
public:
    ip4_protocol ip_prot;
    udp_protocol udp_prot;

    class data_counter counter;

    only_udp_filter() {
        ip_prot.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_prot);
        udp_prot.port_handlers.assign_to_key(PORT, &counter);
    }

    firewall_policy calc_policy(received_msg& msg) override {
        ip_prot.handle_received_event(msg);

        int cnt = counter.get_count();
        counter.reset();

        if (cnt == 1) return ALLOW;
        else return BLOCK;
    }
};
#endif //SERVERCLIENT_ONLY_UDP_FILTER_H
