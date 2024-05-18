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
#include "../../temp_connections/udp_client_server/udp_server.h"


class dns_server : private basic_receiver<socket_msg> {
    udp_server udpServer;

    void handle_received_event(socket_msg &event) override;

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
//    std::map<string, ip4_addr> mappings_type_a = {
//            {"google.com", convert_to_ip4_addr("172.217.22.78")}
//    };

    trie_node<char, 256, assign, string> mappings_type_mx;
//    std::map<string, string> mappings_type_mx = {
//            {"google.com", "smtp.google.com"}
//    };


    dns_server() : udpServer(DNS_SERVER_PORT) {
        udpServer.add_listener(this);
    }
};


#endif //SERVERCLIENT_DNS_SERVER_H
