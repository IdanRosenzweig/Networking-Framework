#include "dns_server.h"

#include <iostream>
using namespace std;

void dns_server::handle_received_event(udp_packet_stack &&event) {
//void dns_server::handle_received_event(emp_packet_stack& event) {
    uint8_t *buff = event.msg.data.data() + event.msg.curr_offset;
    uint8_t * curr_ptr = buff;

    struct dns_header request_header;
    curr_ptr += extract_from_network_order(&request_header, curr_ptr);
    uint16_t no_queries = request_header.query_count;

    vector<dns_query> requested_queries;
    vector<dns_answer> answers;
    for (int i  = 0; i < no_queries; i++) {
        struct dns_query query;
        curr_ptr += extract_from_network_order(&query, curr_ptr, buff);
        requested_queries.push_back(query);

        switch (query.q_type) {
            case DNS_TYPE_A: {
                cout << "received A query for name: " << query.q_name.c_str() << endl;

                auto node = mappings_type_a.search(string((char*) query.q_name.c_str()));
                if (node == nullptr) {
                    cerr << "no mapping for this name" << endl;
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
                cout << "received MX query for name: " << query.q_name.c_str() << endl;

                auto node = mappings_type_mx.search(string((char*) query.q_name.c_str()));
                if (node == nullptr) {
                    cerr << "no mapping for this name" << endl;
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
                cerr << "got query with unknown type" << endl;
                throw;
            }
        }

    }

    // reply
    send_msg msg;
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
    header.auth_count = 0;
    header.add_count = 0;
    curr_reply_buff += write_in_network_order(curr_reply_buff, &header);

    // include the requested queries
    for (struct dns_query& query : requested_queries)
        curr_reply_buff += write_in_network_order(curr_reply_buff, &query);

    // add our answers
    for (struct dns_answer& ans : answers)
        curr_reply_buff += write_in_network_order(curr_reply_buff, &ans);

    msg.set_count(curr_reply_buff - reply_buff);
    udpServer.send_data_to_client(event.source_addr, event.source_port, msg);
//    empServer.send_data_to_client(event.source_addr, event.source_point, msg);
}
