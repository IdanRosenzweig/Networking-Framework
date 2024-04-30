#ifndef SERVERCLIENT_ANALYZER_H
#define SERVERCLIENT_ANALYZER_H

#include "../linux/data_link_layer/data_link_sniffer.h"
#include "../abstract/receiving/msg_receiver.h"

#include "../protocols/ether/ethernet_protocol.h"
#include "../protocols/arp/arp_protocol.h"
#include "../protocols/ip4/ip4_protocol.h"
#include "../protocols/udp/udp_protocol.h"
#include "../protocols/tcp/tcp_protocol.h"
#include "../protocols/icmp/icmp_protocol.h"

#include "packet_displayer.h"

class analyzer : public msg_receiver {
    data_link_sniffer raw_sniffer;

    ethernet_protocol ethernetProtocol;
    arp_protocol arpProtocol;
    ip4_protocol ip4Protocol;
    udp_protocol udpProtocol;
//    tcp_protocol tcpProtocol;
    icmp_protocol icmpProtocol;

    packet_displayer displayer;

public:
    analyzer();

    void handle_received_event(received_msg& event) override;
};


#endif //SERVERCLIENT_ANALYZER_H
