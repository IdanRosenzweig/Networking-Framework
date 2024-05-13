#include <linux/if_ether.h>
#include "net_analyzer.h"
#include <iostream>
using namespace std;


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

