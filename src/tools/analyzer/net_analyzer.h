#ifndef NETWORKING_NET_ANALYZER_H
#define NETWORKING_NET_ANALYZER_H

#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/sniffer/sniffer.h"
#include "../../abstract/sniffer/basic_sniff_handler.h"

#include "../../protocols/ether2/ethernet2_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/tcp/tcp_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"

class net_analyzer {
    // ptr to the sniffer providing the messages
    struct sniffer* sniffer;

    // classes handling whenever sniffed messages arrives
    class outgoing_sniff : public basic_sniff_handler {
        net_analyzer* master;
    public:
        explicit outgoing_sniff(net_analyzer *master) : master(master) {}

        void handle_callback(recv_msg_t &&data) override;

    };
    outgoing_sniff outgoing_sniff;

    class incoming_sniff : public basic_sniff_handler {
        net_analyzer* master;
    public:
        explicit incoming_sniff(net_analyzer *master) : master(master) {}

        void handle_callback(recv_msg_t &&data) override;
    };

    incoming_sniff incoming_sniff;

    // actual protocols that will process sniffed messages
    ethernet2_protocol ethernetProtocol;
    class ether_handler : public msg_recv_listener {
    public:
        void handle_callback(recv_msg_t &&data) override;
    } etherHandler;

//    net_arp arpProtocol;

    ip4_protocol ip4Protocol;
    class ip4_handler : public msg_recv_listener {
    public:
        void handle_callback(recv_msg_t &&data) override;
    } ip4Handler;

    udp_protocol udpProtocol;
    class udp_handler : public msg_recv_listener {
    public:
        void handle_callback(recv_msg_t &&data) override;
    } udpHandler;

//    tcp_protocol tcpProtocol;

    icmp_protocol icmpProtocol;
    class icmp_handler : public msg_recv_listener {
    public:
        void handle_callback(recv_msg_t &&data) override;
    } icmpHandler;

public:
    net_analyzer(struct sniffer* sniffer);

};


#endif //NETWORKING_NET_ANALYZER_H
