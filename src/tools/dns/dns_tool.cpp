#include "dns_tool.h"

#include <iostream>
#include <istream>
#include <string>
#include <sstream>
using namespace std;

#include "lib/hexdump.h"

#include "src/abstract/receiving/recv_queue.h"


// dns_server::dns_server(ip4_addr src_ip, gateway *network_layer_gw) : udpServer(DNS_SERVER_PORT, src_ip, network_layer_gw) {
//     udpServer.add_listener(this);
// //dns_server::dns_server(ip4_addr src_ip, gateway *network_layer_gw) : empServer({0x53}, src_ip, network_layer_gw) {
// //    empServer.add_listener(this);


//         // default start of authority
//     my_start_of_authority.name = (uint8_t*) "some local dns server";
//     my_start_of_authority.type = DNS_TYPE_SOA;
//     my_start_of_authority._class = DNS_CLASS_IN;
//     my_start_of_authority.ttl = 60 * 15;

//     soa_rdata_t rdata;
//     rdata.primary_server = (uint8_t*) "none";
//     rdata.admin_mail_server = (uint8_t*) "none";
//     rdata.serial_num = 69;
//     rdata.refresh_interval = 60 * 30;
//     rdata.retry_interval = 60 * 15;
//     rdata.expire_limit = 7 * 24 * 60 * 60;
//     rdata.max_ttl = 1 * 24 * 60 * 60;

//     my_start_of_authority.rdata = convert_to_rdata(&rdata);
//     my_start_of_authority.data_len = my_start_of_authority.rdata.size();
// }

// void dns_server::handle_callback(udp_packet_stack &&data) {
// //void dns_server::handle_callback(emp_packet_stack &&data) {
//     uint8_t *buff = data.plain_data.buff_at_curr_offset();
//     uint8_t * curr_ptr = buff;

//     struct dns_header request_header;
//     curr_ptr += extract_from_network_order(&request_header, curr_ptr);
//     uint16_t no_queries = request_header.query_count;

//     vector<dns_query> requested_queries;
//     vector<dns_answer> answers;
//     vector<dns_answer> auth_ans;
//     for (int i  = 0; i < no_queries; i++) {
//         struct dns_query query;
//         curr_ptr += extract_from_network_order(&query, curr_ptr, buff);
//         requested_queries.push_back(query);

//         switch (query.q_type) {
//             case DNS_TYPE_A: {
//                 std::cout << "received A query for name: " << query.q_name.c_str() << endl;

//                 auto node = mappings_type_a.search(string((char*) query.q_name.c_str()));
//                 if (node == nullptr) {
//                     std::cerr << "no mapping for this name" << endl;
//                     // just send my start of authority
//                     auth_ans.push_back(my_start_of_authority);
//                     break;
//                 }

//                 struct dns_answer curr_ans;
//                 curr_ans.name = query.q_name;
//                 curr_ans.type = query.q_type;
//                 curr_ans._class = query.q_class;
//                 curr_ans.ttl = 3600; // one hour
//                 curr_ans.data_len = 4;

//                 curr_ans.rdata = ustring (4, 0);
//                 ip4_addr addr = node->key;
//                 write_in_network_order((uint8_t*) curr_ans.rdata.c_str(), &addr);

//                 answers.push_back(std::move(curr_ans));
//                 break;
//             }
//             case DNS_TYPE_MX: {
//                 std::cout << "received MX query for name: " << query.q_name.c_str() << endl;

//                 auto node = mappings_type_mx.search(string((char*) query.q_name.c_str()));
//                 if (node == nullptr) {
//                     std::cerr << "no mapping for this name" << endl;
//                     // just send my start of authority
//                     auth_ans.push_back(my_start_of_authority);
//                     break;
//                 }

//                 struct dns_answer curr_ans;
//                 curr_ans.name = query.q_name;
//                 curr_ans.type = query.q_type;
//                 curr_ans._class = query.q_class;
//                 curr_ans.ttl = 3600; // one hour

//                 mx_rdata_t rdata{10, ustring((uint8_t*) node->key.c_str()) };
//                 curr_ans.rdata = convert_to_rdata(&rdata);
//                 curr_ans.data_len = curr_ans.rdata.size();

//                 answers.push_back(std::move(curr_ans));
//                 break;
//             }
//             default: {
//                 std::cerr << "received query of unsupported type" << endl;
//                 // just send my start of authority
//                 auth_ans.push_back(my_start_of_authority);
//                 break;
//             }
//         }

//     }

//     // reply
//     send_msg_t msg;
//     uint8_t * reply_buff = msg.get_active_buff();
//     uint8_t * curr_reply_buff = reply_buff;

//     // header
//     struct dns_header header;
//     header.id = request_header.id;
//     header.qr = 1; // this is a response
//     header.opcode = 0; // standard
//     header.aa = 0;
//     header.tc = 0;
//     header.rd = 0; // don't want to start handle recursion...
//     header.ra = 0;
// //    header.z = 0;
// //    header.ad = 0;
// //    header.cd = 0;
//     header.rcode = 0;
//     header.query_count = no_queries;
//     header.ans_count = answers.size();
//     header.auth_count = auth_ans.size();
//     header.add_count = 0;
//     curr_reply_buff += write_in_network_order(curr_reply_buff, &header);

//     // include the requested queries
//     for (struct dns_query& query : requested_queries)
//         curr_reply_buff += write_in_network_order(curr_reply_buff, &query);

//     // add our answers
//     for (struct dns_answer& ans : answers)
//         curr_reply_buff += write_in_network_order(curr_reply_buff, &ans);

//     // add the auth answers
//     for (struct dns_answer& ans : auth_ans)
//         curr_reply_buff += write_in_network_order(curr_reply_buff, &ans);

//     msg.set_count(curr_reply_buff - reply_buff);
//     udpServer.send_data_to_client(data.source_addr, data.source_port, std::move(msg));
// //    empServer.send_data_to_client(data.source_addr, data.source_point, std::move(msg));
// }



void dns_tool::send_requests(shared_ptr<net_access> const& dns_surface, struct dns_query const& query) {
    // listen to dns messages via a queue
    shared_ptr<recv_queue<dns_message>> queue = make_shared<recv_queue<dns_message>>();
    dns_protocol::connect_recv(dns_surface, queue);

    // prepare the dns message
    struct dns_message msg;

    // header
    msg.header.id = 0x8950; // todo control this value? do random?
    msg.header.qr = 0; // this is a request
    msg.header.opcode = 0; // standard
    msg.header.aa = 0;
    msg.header.tc = 0;
    msg.header.rd = 1; // no recursion
    msg.header.ra = 0;
//    msg.header.z = 0;
//    msg.header.ad = 0;
//    msg.header.cd = 0;
    msg.header.rcode = 0;
    msg.header.query_count = 1; // the number of queries
    msg.header.ans_count = 0;
    msg.header.auth_count = 0;
    msg.header.add_count = 0;

    // add the query
    msg.queries.push_back(query);

    while (true) {
        // send the message
        dns_protocol::send(dns_surface, msg);

        // recv reply
        recv_reply: {}
        auto* reply = queue->queue.front(std::chrono::milliseconds(1000)); // todo be able to control this value
        if (reply == nullptr) {cout << "no valid reply, resending" << endl; continue;} // no response in a reasonable time, continue and send again
        queue->queue.pop_front();

        // process reply
        struct dns_message dns_reply = *reply;

        if (msg.header.id != dns_reply.header.id) goto recv_reply; // not our reply

        // print the reply
        print_dns_message(&dns_reply, false, true);

        break;
    }

}

struct dns_answer dns_tool::get_my_start_of_authority() {
    static dns_answer my_start_of_authority;
    static bool inited = false;
    if (!inited) {
        static string name = "some local dns server";
        my_start_of_authority.name = vector<uint8_t>(name.data(), name.data() + name.size() + 1);
        my_start_of_authority.type = dns_record_type::soa;
        my_start_of_authority._class = dns_record_class::in;
        my_start_of_authority.ttl = 60 * 15;

        soa_rdata_t rdata;
        static string primary_server = "none";
        rdata.primary_server = vector<uint8_t>(primary_server.data(), primary_server.data() + primary_server.size() + 1);
        static string admin_mail_server = "none";
        rdata.admin_mail_server = vector<uint8_t>(admin_mail_server.data(), admin_mail_server.data() + admin_mail_server.size() + 1);
        rdata.serial_num = 69;
        rdata.refresh_interval = 60 * 30;
        rdata.retry_interval = 60 * 15;
        rdata.expire_limit = 7 * 24 * 60 * 60;
        rdata.max_ttl = 1 * 24 * 60 * 60;

        my_start_of_authority.rdata = convert_to_rdata(&rdata);
        my_start_of_authority.data_len = my_start_of_authority.rdata.size();
    }

    return my_start_of_authority;
}

struct dns_message dns_tool::handle_query(dns_database* database, struct dns_message const* msg) {
    vector<dns_answer> answers;
    vector<dns_answer> auth_ans;
    for (int i  = 0; i < msg->header.query_count; i++) {
        struct dns_query const& query = msg->queries[i];

        switch (query.q_type) {
            case dns_record_type::a: {
                // cout << "received query type (a) for name: " << (char*) query.q_name.data() << endl;

                ip4_addr res;
                if (auto tmp = database->search_record_a(string((char*) query.q_name.data())); tmp.has_value()) res = tmp.value();
                else {
                    // cout << "no record for this name" << endl;
                    // just send my start of authority
                    auth_ans.push_back(get_my_start_of_authority());
                    break;
                }

                struct dns_answer curr_ans;
                curr_ans.name = query.q_name;
                curr_ans.type = query.q_type;
                curr_ans._class = query.q_class;
                curr_ans.ttl = 3600; // one hour
                curr_ans.data_len = 4;

                curr_ans.rdata = vector<uint8_t>(4, 0);
                write_in_network_order(curr_ans.rdata.data(), &res);

                answers.push_back(std::move(curr_ans));
                break;
            }
            case dns_record_type::mx: {
                // cout << "received query type (mx) for name: " << (char*) query.q_name.data() << endl;

                string res;
                if (auto tmp = database->search_record_mx(string((char*) query.q_name.data())); tmp.has_value()) res = tmp.value();
                else {
                    // cout << "no record for this name" << endl;
                    // just send my start of authority
                    auth_ans.push_back(get_my_start_of_authority());
                    break;
                }

                struct dns_answer curr_ans;
                curr_ans.name = query.q_name;
                curr_ans.type = query.q_type;
                curr_ans._class = query.q_class;
                curr_ans.ttl = 3600; // one hour

                mx_rdata_t rdata{10, vector<uint8_t>(res.data(), res.data() + res.size())};
                curr_ans.rdata = convert_to_rdata(&rdata);
                curr_ans.data_len = curr_ans.rdata.size();

                answers.push_back(std::move(curr_ans));
                break;
            }
            default: {
                // cout << "received query type (unsupported)" << endl;
                // just send my start of authority
                auth_ans.push_back(get_my_start_of_authority());
                break;
            }
        }

    }

    // create the reply
    struct dns_message reply;

    // header
    reply.header.id = msg->header.id; // same id
    reply.header.qr = 1; // this is a response
    reply.header.opcode = 0; // standard
    reply.header.aa = 0;
    reply.header.tc = 0;
    reply.header.rd = 0; // no recursion
    reply.header.ra = 0;
//    header.z = 0;
//    header.ad = 0;
//    header.cd = 0;
    reply.header.rcode = 0;
    reply.header.query_count = msg->header.query_count; // replay the queries
    reply.header.ans_count = answers.size();
    reply.header.auth_count = auth_ans.size();
    reply.header.add_count = 0;

    // queries and answers
    reply.queries = msg->queries; // replay the queries
    reply.ans = answers;
    reply.ans_auth = auth_ans;

    return reply;
}
