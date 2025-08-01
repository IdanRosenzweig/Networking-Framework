#include "net_analyzer.h"

#include <iostream>
using namespace std;

void net_analyzer::ether_handler::handle_callback(recv_msg_t &&data) {
    std::cout << "offset: 0x" << hex << data.protocol_offsets.back().first << "\t";
    std::cout << "ethernet\t";

    struct ethernet2_header header;
    extract_from_network_order(&header, data.buff_of_prot_from_end(0));
    std::cout << "src " << convert_to_str(header.src_addr) << "\t";
    std::cout << "dest " << convert_to_str(header.dest_addr) << "\t";

    std::cout << endl;
}

void net_analyzer::ip4_handler::handle_callback(recv_msg_t &&data) {
    std::cout << "offset: 0x" << hex << data.protocol_offsets.back().first << "\t";
    std::cout << "ip4\t\t";

    struct ip4_header header;
    extract_from_network_order(&header, data.buff_of_prot_from_end(0));
    std::cout << "src " << convert_to_str(header.source_addr) << "\t";
    std::cout << "dest " << convert_to_str(header.dest_addr) << "\t";

    std::cout << endl;
}

void net_analyzer::udp_handler::handle_callback(recv_msg_t &&data) {
    std::cout << "offset: 0x" << hex << data.protocol_offsets.back().first << "\t";
    std::cout << "udp\t\t";

    struct udp_header header;
    extract_from_network_order(&header,  data.buff_of_prot_from_end(0));
    std::cout << "src port " << dec << header.source_port << "\t\t";
    std::cout << "dest port " << dec << header.dest_port << "\t";

    std::cout << endl;
}

void net_analyzer::icmp_handler::handle_callback(recv_msg_t &&data) {
    std::cout << "offset: 0x" << hex << data.protocol_offsets.back().first << "\t";
    std::cout << "icmp\t";

    struct icmp_header header;
    extract_from_network_order(&header, data.buff_of_prot_from_end(0));

    std::cout << endl;
}

net_analyzer::net_analyzer(struct sniffer* _sniffer) : sniffer(_sniffer), outgoing_sniff(this), incoming_sniff(this) {
    sniffer->outgoing.sniffers.push_back(&outgoing_sniff);
    sniffer->incoming.sniffers.push_back(&incoming_sniff);

    ethernetProtocol.default_listener = &etherHandler;
    ethernetProtocol.listeners.append_new_empty_handler(&ip4Protocol);
    ethernetProtocol.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_PROTOCOL_INDEX>(ethernet2_header::ethertype_values::ip4);
//    ethernetProtocol.listeners.append_new_empty_handler(&arpProtocol);
//    ethernetProtocol.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_PROTOCOL_INDEX>(htons(ETH_P_ARP));

//    arpProtocol.default_listener = &arpHandler;

    ip4Protocol.default_listener = &ip4Handler;
    ip4Protocol.listeners.append_new_empty_handler(&udpProtocol);
    ip4Protocol.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::udp);
    ip4Protocol.listeners.append_new_empty_handler(&icmpProtocol);
    ip4Protocol.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::icmp);
//    ip4Protocol.listeners.append_new_empty_handler(&tcpProtocol);
//    ip4Protocol.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::tcp);

    udpProtocol.default_listener = &udpHandler;

    icmpProtocol.default_listener = &icmpHandler;

}

void net_analyzer::outgoing_sniff::handle_callback(recv_msg_t &&data) {
    std::cout << "##### outgoing packet" << endl;
    std::cout << "##### protocol stack" << endl;
    recv_msg_t copy(data);
    master->ethernetProtocol.handle_callback(std::move(copy));

    std::cout << "##### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(data.buff.data(), 1, data.buff.size(), pipe);
    pclose(pipe);

    std::cout << endl;
}

void net_analyzer::incoming_sniff::handle_callback(recv_msg_t &&data) {
    std::cout << "##### incoming packet" << endl;
    std::cout << "### protocol stack" << endl;
    recv_msg_t copy(data);
    master->ethernetProtocol.handle_callback(std::move(copy));

    std::cout << "### hex dump" << endl;
    FILE* pipe = popen("/usr/bin/hd", "w");
    fwrite(data.buff.data(), 1, data.buff.size(), pipe);
    pclose(pipe);

    std::cout << endl;
}
