#include "net_analyzer.h"

#include <linux/if_ether.h>
#include <linux/ip.h>

#include <iostream>
using namespace std;

void net_analyzer::ether_handler::handle_received_event(received_msg &event) {
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

void net_analyzer::ip4_handler::handle_received_event(received_msg &event) {
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

void net_analyzer::udp_handler::handle_received_event(received_msg &event) {
    cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    cout << "udp\t\t";

    struct udp_header* udp = (struct udp_header*) (event.data.get() + event.protocol_offsets.back().first);
    cout << "src port " << dec << ntohs(udp->source_port) << "\t\t";
    cout << "dest port " << dec << ntohs(udp->dest_port) << "\t";

    cout << endl;
}

void net_analyzer::icmp_handler::handle_received_event(received_msg &event) {
    cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    cout << "icmp\t";

    struct icmp_header* icmp = (struct icmp_header*) (event.data.get() + event.protocol_offsets.back().first);

    cout << endl;
}

net_analyzer::net_analyzer(const string& interface) : raw_sniffer(interface) {
    raw_sniffer.add_sniffer((basic_sniffer*) this);

    ethernetProtocol.default_handler = &etherHandler;
    ethernetProtocol.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip4Protocol);
//    ethernetProtocol.protocol_handlers.assign_to_key(htons(ETH_P_ARP), &arpProtocol);

//    arpProtocol.default_handler = &displayer;

    ip4Protocol.default_handler = &ip4Handler;
    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_UDP, &udpProtocol);
//    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_TCP, &tcpProtocol);
    ip4Protocol.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmpProtocol);

    udpProtocol.default_handler = &udpHandler;
//    udpProtocol.default_handler = &displayer;

    icmpProtocol.default_handler = &icmpHandler;
//    icmpProtocol.default_handler = &displayer;
}

void net_analyzer::handle_outgoing_packet(received_msg &msg) {
    cout << "##### outgoing packet" << endl;
    cout << "##### protocol stack" << endl;
    ethernetProtocol.handle_received_event(msg);

    cout << "##### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(msg.data.get(), 1, msg.sz, pipe);
    pclose(pipe);

    cout << endl;

}

void net_analyzer::handle_incoming_packet(received_msg &msg) {
    cout << "##### incoming packet" << endl;
    cout << "### protocol stack" << endl;
    ethernetProtocol.handle_received_event(msg);

    cout << "### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(msg.data.get(), 1, msg.sz, pipe);
    pclose(pipe);

    cout << endl;
}

