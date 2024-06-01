#include "dns_server.h"

#include <iostream>
using namespace std;

dns_server::dns_server(ip4_addr src_ip, gateway *network_layer_gw) : udpServer(DNS_SERVER_PORT, src_ip, network_layer_gw) {
    udpServer.add_listener(this);
//dns_server::dns_server(ip4_addr src_ip, gateway *network_layer_gw) : empServer({0x53}, src_ip, network_layer_gw) {
//    empServer.add_listener(this);


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

void dns_server::handle_callback(udp_packet_stack &&data) {
//void dns_server::handle_callback(emp_packet_stack &&data) {
    uint8_t *buff = data.plain_data.buff_at_curr_offset();
    uint8_t * curr_ptr = buff;

    struct dns_header request_header;
    curr_ptr += extract_from_network_order(&request_header, curr_ptr);
    uint16_t no_queries = request_header.query_count;

    vector<dns_query> requested_queries;
    vector<dns_answer> answers;
    vector<dns_answer> auth_ans;
    for (int i  = 0; i < no_queries; i++) {
        struct dns_query query;
        curr_ptr += extract_from_network_order(&query, curr_ptr, buff);
        requested_queries.push_back(query);

        switch (query.q_type) {
            case DNS_TYPE_A: {
                std::cout << "received A query for name: " << query.q_name.c_str() << endl;

                auto node = mappings_type_a.search(string((char*) query.q_name.c_str()));
                if (node == nullptr) {
                    std::cerr << "no mapping for this name" << endl;
                    // just send my start of authority
                    auth_ans.push_back(my_start_of_authority);
                    break;
                }

                struct dns_answer curr_ans;
                curr_ans.name = query.q_name;
                curr_ans.type = query.q_type;
                curr_ans._class = query.q_class;
                curr_ans.ttl = 3600; // one hour
                curr_ans.data_len = 4;

                curr_ans.rdata = ustring (4, 0);
                ip4_addr addr = node->key;
                write_in_network_order((uint8_t*) curr_ans.rdata.c_str(), &addr);

                answers.push_back(std::move(curr_ans));
                break;
            }
            case DNS_TYPE_MX: {
                std::cout << "received MX query for name: " << query.q_name.c_str() << endl;

                auto node = mappings_type_mx.search(string((char*) query.q_name.c_str()));
                if (node == nullptr) {
                    std::cerr << "no mapping for this name" << endl;
                    // just send my start of authority
                    auth_ans.push_back(my_start_of_authority);
                    break;
                }

                struct dns_answer curr_ans;
                curr_ans.name = query.q_name;
                curr_ans.type = query.q_type;
                curr_ans._class = query.q_class;
                curr_ans.ttl = 3600; // one hour

                mx_rdata_t rdata{10, ustring((uint8_t*) node->key.c_str()) };
                curr_ans.rdata = convert_to_rdata(&rdata);
                curr_ans.data_len = curr_ans.rdata.size();

                answers.push_back(std::move(curr_ans));
                break;
            }
            default: {
                std::cerr << "received query of unsupported type" << endl;
                // just send my start of authority
                auth_ans.push_back(my_start_of_authority);
                break;
            }
        }

    }

    // reply
    send_msg_t msg;
    uint8_t * reply_buff = msg.get_active_buff();
    uint8_t * curr_reply_buff = reply_buff;

    // header
    struct dns_header header;
    header.id = request_header.id;
    header.qr = 1; // this is a response
    header.opcode = 0; // standard
    header.aa = 0;
    header.tc = 0;
    header.rd = 0; // don't want to start handle recursion...
    header.ra = 0;
//    header.z = 0;
//    header.ad = 0;
//    header.cd = 0;
    header.rcode = 0;
    header.query_count = no_queries;
    header.ans_count = answers.size();
    header.auth_count = auth_ans.size();
    header.add_count = 0;
    curr_reply_buff += write_in_network_order(curr_reply_buff, &header);

    // include the requested queries
    for (struct dns_query& query : requested_queries)
        curr_reply_buff += write_in_network_order(curr_reply_buff, &query);

    // add our answers
    for (struct dns_answer& ans : answers)
        curr_reply_buff += write_in_network_order(curr_reply_buff, &ans);

    // add the auth answers
    for (struct dns_answer& ans : auth_ans)
        curr_reply_buff += write_in_network_order(curr_reply_buff, &ans);

    msg.set_count(curr_reply_buff - reply_buff);
    udpServer.send_data_to_client(data.source_addr, data.source_port, std::move(msg));
//    empServer.send_data_to_client(data.source_addr, data.source_point, std::move(msg));
}


void add_a_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string ip;
    input >> ip;

    auto node = server->mappings_type_a.add_word(domain);
    node->key = convert_to_ip4_addr(ip);
}

void add_mx_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string server_domain;
    input >> server_domain;

    auto node = server->mappings_type_mx.add_word(domain);
    node->key = server_domain;
}


void load_database(dns_server *server, const string& db_path) {
    std::ifstream input(db_path);
    while (true) {
        if (input.eof()) break;

        string type;
        input >> type;

        if (type.empty()) continue;

        if (type == "a")
            add_a_record(server, input);
        else if (type == "mx")
            add_mx_record(server, input);
        else {
            std::cerr << "invalid type of dns record" << endl;
            break;
        }

    }
    std::cout << "finished loading dns database" << endl;
}