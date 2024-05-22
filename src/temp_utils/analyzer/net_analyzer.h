#ifndef SERVERCLIENT_NET_ANALYZER_H
#define SERVERCLIENT_NET_ANALYZER_H

#include "../../linux/interface_sniffer.h"
#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/sniffer/msg_sniffer.h"

#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/tcp/tcp_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"

class net_analyzer : public msg_sniffer {
    interface_sniffer raw_sniffer;

    ethernet_protocol ethernetProtocol;
    class ether_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &&event) override;
    } etherHandler;

//    net_arp arpProtocol;

    ip4_protocol ip4Protocol;
    class ip4_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &&event) override;
    } ip4Handler;

    udp_protocol udpProtocol;
    class udp_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &&event) override;
    } udpHandler;

//    tcp_protocol tcpProtocol;

    icmp_protocol icmpProtocol;
    class icmp_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &&event) override;
    } icmpHandler;

public:
    net_analyzer(const string& interface);

    void handle_outgoing_packet(const received_msg &msg) override;

    void handle_incoming_packet(const received_msg &msg) override;
};


#endif //SERVERCLIENT_NET_ANALYZER_H
