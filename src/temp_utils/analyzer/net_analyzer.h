#ifndef SERVERCLIENT_NET_ANALYZER_H
#define SERVERCLIENT_NET_ANALYZER_H

#include "../../linux/data_link_layer/data_link_sniffer.h"
#include "../../linux/data_link_layer/interface_sniffer.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/connection/basic_sniffer.h"

#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/tcp/tcp_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"

class net_analyzer : public basic_sniffer {
    interface_sniffer raw_sniffer;

    ethernet_protocol ethernetProtocol;
    class ether_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &event) override {
            cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
            cout << "ethernet\t";

            struct ether_header* ether = (struct ether_header*) (event.data.get() + event.protocol_offsets.back().first);
            mac_addr src;
            mac_addr dest;
            memcpy(&src, ether->ether_shost, ETH_ALEN);
            memcpy(&dest, ether->ether_dhost, ETH_ALEN);
            cout << "src "; print_mac(src); cout << "\t";
            cout << "dest "; print_mac(dest); cout << "\t";

            cout << endl;
        }
    } etherHandler;

//    net_arp arpProtocol;

    ip4_protocol ip4Protocol;
    class ip4_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &event) override {
            cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
            cout << "ip4\t\t";

            struct iphdr* ip_hdr = (struct iphdr*) (event.data.get() + event.protocol_offsets.back().first);
            ip4_addr src;
            ip4_addr dest;
            extract_from_network_order(&src, (uint8_t*) &ip_hdr->saddr);
            extract_from_network_order(&dest, (uint8_t*) &ip_hdr->daddr);
            cout << "src " << convert_to_str(src) << "\t";
            cout << "dest " << convert_to_str(dest) << "\t";

            cout << endl;
        }
    } ip4Handler;

    udp_protocol udpProtocol;
    class udp_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &event) override {
            cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
            cout << "udp\t\t";

            struct udp_header* udp = (struct udp_header*) (event.data.get() + event.protocol_offsets.back().first);
            cout << "src port " << dec << ntohs(udp->source_port) << "\t\t";
            cout << "dest port " << dec << ntohs(udp->dest_port) << "\t";

            cout << endl;
        }
    } udpHandler;

//    tcp_protocol tcpProtocol;

    icmp_protocol icmpProtocol;
    class icmp_handler : public msg_receiver {
    public:
        void handle_received_event(received_msg &event) override {
            cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
            cout << "icmp\t";

            struct icmp_header* icmp = (struct icmp_header*) (event.data.get() + event.protocol_offsets.back().first);

            cout << endl;
        }
    } icmpHandler;

//    packet_displayer displayer;

public:
    net_analyzer(const string& interface);

    void handle_outgoing_packet(received_msg &msg) override;

    void handle_incoming_packet(received_msg &msg) override;
};


#endif //SERVERCLIENT_NET_ANALYZER_H
