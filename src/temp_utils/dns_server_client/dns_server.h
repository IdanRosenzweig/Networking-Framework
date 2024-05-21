#ifndef SERVERCLIENT_DNS_SERVER_H
#define SERVERCLIENT_DNS_SERVER_H

#include <fstream>
#include <string>

#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/utils/trie.h"

#include "dns.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../temp_prot_stacks/udp_client_server/udp_server.h"
#include "../../temp_prot_stacks/emp/emp_server.h"


class dns_server : private basic_receiver<udp_packet_stack> {
//class dns_server : private basic_receiver<emp_packet_stack> {

    udp_server udpServer;
//    emp_server empServer;

    void handle_received_event(udp_packet_stack &event) override;
//    void handle_received_event(emp_packet_stack &event) override;

public:
    // valid dns hostname chars are a-z, A-Z, 0-9, and hyphen
    static constexpr auto assign = [](char val) -> size_t {
        if ('a' <= val && val <= 'z') return val - 'a';
        else if ('A' <= val && val <= 'Z')return ('z' - 'a' + 1) + val - 'A';
        else if ('0' <= val && val <= '9') return ('z' - 'a' + 1) + ('Z' - 'A' + 1) + val - '0';
        else
            return ('z' - 'a' + 1) + ('Z' - 'A' + 1) + ('9' - '0' + 1);
    };
    trie_node<char, 256, assign, ip4_addr> mappings_type_a;

    trie_node<char, 256, assign, string> mappings_type_mx;

    dns_server(ip4_addr src_ip, msg_gateway* network_layer_gw) : udpServer(DNS_SERVER_PORT, src_ip, network_layer_gw) {
        udpServer.add_listener(this);
    }
//    dns_server() : empServer({0x53}) {
//        empServer.add_listener(this);
//    }
};


#endif //SERVERCLIENT_DNS_SERVER_H
