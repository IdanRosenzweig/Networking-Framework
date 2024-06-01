#ifndef NETWORKING_DNS_SERVER_H
#define NETWORKING_DNS_SERVER_H

#include <fstream>
#include <string>

#include "dns.h"

#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/utils/trie.h"

#include "../../direct_connections/udp/udp_server.h"
#include "../../direct_connections/emp/emp_server.h"

class dns_server : private basic_recv_listener<udp_packet_stack> {
//class dns_server : private basic_recv_listener<emp_packet_stack> {

    udp_server udpServer;
//    emp_server empServer;

    void handle_callback(udp_packet_stack &&data) override;
//    void handle_callback(emp_packet_stack &&event) override;

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

    dns_answer my_start_of_authority;

    dns_server(ip4_addr src_ip, gateway* network_layer_gw);
};

// add records to server
void add_a_record(dns_server *server, std::ifstream &input);

void add_mx_record(dns_server *server, std::ifstream &input);

// load entire record database to server
void load_database(dns_server* server, const string& db_path);

#endif //NETWORKING_DNS_SERVER_H
