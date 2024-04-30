#include <linux/if_ether.h>
#include "analyzer.h"
#include <iostream>
using namespace std;


analyzer::analyzer() : raw_sniffer(true) {
    raw_sniffer.add_listener(this);

    ethernetProtocol.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip4Protocol);
    ethernetProtocol.protocol_handlers.assign_to_key(htons(ETH_P_ARP), &arpProtocol);

    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_UDP, &udpProtocol);
//    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_TCP, &tcpProtocol);
    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmpProtocol);

    // endpoints
    arpProtocol.default_handler = &displayer;
    udpProtocol.default_handler = &displayer;
    icmpProtocol.default_handler = &displayer;
}

void analyzer::handle_received_event(received_msg& event) {
    ethernetProtocol.handle_received_event(event); // assuming all traffic on data link layer uses ethernet
}
