#ifndef NETWORKING_DNS_SERVER_H
#define NETWORKING_DNS_SERVER_H

#include <fstream>
#include <string>

#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/utils/trie.h"

#include "dns.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../temp_prot_stacks/udp_client_server/udp_server.h"
#include "../../temp_prot_stacks/emp/emp_server.h"


class dns_server : private basic_recv_listener<udp_packet_stack> {
//class dns_server : private basic_recv_listener<emp_packet_stack> {

    udp_server udpServer;
//    emp_server empServer;

    void handle_received_event(udp_packet_stack &&event) override;
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

    dns_answer my_start_of_authority;

    dns_server(ip4_addr src_ip, gateway* network_layer_gw) : udpServer(DNS_SERVER_PORT, src_ip, network_layer_gw) {
        udpServer.add_listener(this);

        // default start of authority
        my_start_of_authority.name = (uint8_t*) "some local dns server";
        my_start_of_authority.type = DNS_TYPE_SOA;
        my_start_of_authority._class = DNS_CLASS_IN;
        my_start_of_authority.ttl = 60 * 15;

        soa_rdata_t rdata;
        rdata.primary_server = (uint8_t*) "none";
        rdata.admin_mail_server = (uint8_t*) "none";
        rdata.serial_num = 69;
        rdata.refresh_interval = 60 * 30;
        rdata.retry_interval = 60 * 15;
        rdata.expire_limit = 7 * 24 * 60 * 60;
        rdata.max_ttl = 1 * 24 * 60 * 60;

        my_start_of_authority.rdata = convert_to_rdata(&rdata);
        my_start_of_authority.data_len = my_start_of_authority.rdata.size();
    }
//    dns_server() : empServer({0x53}) {
//        empServer.add_listener(this);
//    }
};

// add records to server
void add_a_record(dns_server *server, std::ifstream &input);

void add_mx_record(dns_server *server, std::ifstream &input);

// load entire record database to server
void load_database(dns_server* server, const string& db_path);

#endif //NETWORKING_DNS_SERVER_H
