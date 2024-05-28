#include "net_analyzer.h"

#include <linux/if_ether.h>
#include <linux/ip.h>

#include <iostream>
using namespace std;

void net_analyzer::ether_handler::handle_received_event(received_msg &&event) {
    std::cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    std::cout << "ethernet\t";

    struct ether_header* ether = (struct ether_header*) (event.data.data() + event.protocol_offsets.back().first);
    mac_addr src;
    mac_addr dest;
    memcpy(&src, ether->ether_shost, ETH_ALEN);
    memcpy(&dest, ether->ether_dhost, ETH_ALEN);
    std::cout << "src " << convert_to_str(src) << "\t";
    std::cout << "dest " << convert_to_str(dest) << "\t";

    std::cout << endl;
}

void net_analyzer::ip4_handler::handle_received_event(received_msg &&event) {
    std::cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    std::cout << "ip4\t\t";

    struct iphdr* ip_hdr = (struct iphdr*) (event.data.data() + event.protocol_offsets.back().first);
    ip4_addr src;
    ip4_addr dest;
    extract_from_network_order(&src, (uint8_t*) &ip_hdr->saddr);
    extract_from_network_order(&dest, (uint8_t*) &ip_hdr->daddr);
    std::cout << "src " << convert_to_str(src) << "\t";
    std::cout << "dest " << convert_to_str(dest) << "\t";

    std::cout << endl;
}

void net_analyzer::udp_handler::handle_received_event(received_msg &&event) {
    std::cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    std::cout << "udp\t\t";

    struct udp_header* udp = (struct udp_header*) (event.data.data() + event.protocol_offsets.back().first);
    std::cout << "src port " << dec << ntohs(udp->source_port) << "\t\t";
    std::cout << "dest port " << dec << ntohs(udp->dest_port) << "\t";

    std::cout << endl;
}

void net_analyzer::icmp_handler::handle_received_event(received_msg &&event) {
    std::cout << "offset: 0x" << hex << event.protocol_offsets.back().first << "\t";
    std::cout << "icmp\t";

    struct icmp_header* icmp = (struct icmp_header*) (event.data.data() + event.protocol_offsets.back().first);

    std::cout << endl;
}

net_analyzer::net_analyzer(struct sniffer* _sniffer) : sniffer(_sniffer), outgoing_sniff(this), incoming_sniff(this) {

    ethernetProtocol.default_handler = &etherHandler;
    ethernetProtocol.protocol_handlers[htons(ETH_P_IP)].push_back( &ip4Protocol);
//    ethernetProtocol.protocol_handlers[htons(ETH_P_ARP)].push_back( &arpProtocol);

//    arpProtocol.default_handler = &displayer;

    ip4Protocol.default_handler = &ip4Handler;
    ip4Protocol.protocol_handlers[IPPROTO_UDP].push_back( &udpProtocol);
//    ip4Protocol.protocol_handlers[IPPROTO_TCP].push_back( &tcpProtocol);
    ip4Protocol.protocol_handlers[IPPROTO_ICMP].push_back( &icmpProtocol);

    udpProtocol.default_handler = &udpHandler;
//    udpProtocol.default_handler = &displayer;

    icmpProtocol.default_handler = &icmpHandler;
//    icmpProtocol.default_handler = &displayer;
}

void net_analyzer::outgoing_sniff::handle_received_event(received_msg &&event) {
    std::cout << "##### outgoing packet" << endl;
    std::cout << "##### protocol stack" << endl;
    received_msg copy(event);
    master->ethernetProtocol.handle_received_event(std::move(copy));

    std::cout << "##### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(event.data.data(), 1, event.data.size(), pipe);
    pclose(pipe);

    std::cout << endl;
}

void net_analyzer::incoming_sniff::handle_received_event(received_msg &&event) {
    std::cout << "##### incoming packet" << endl;
    std::cout << "### protocol stack" << endl;
    received_msg copy(event);
    master->ethernetProtocol.handle_received_event(std::move(copy));

    std::cout << "### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(event.data.data(), 1, event.data.size(), pipe);
    pclose(pipe);

    std::cout << endl;
}
