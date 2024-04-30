#include "dns_server.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
using namespace std;

void dns_server::handle_received_event(socket_msg& event) {
    uint8_t *buff = event.msg.data.get() + event.msg.curr_offset;

    struct dns_header *request = (struct dns_header *) buff;
    int no_queries = ntohs(request->query_count);

    cout << "received " << no_queries << " queries" << endl;


#define BUFF_SZ 1024
    char reply_buff[BUFF_SZ];
    memset(reply_buff, 0, BUFF_SZ);
    memcpy(reply_buff, buff, event.msg.sz - event.msg.curr_offset);

    uint8_t *curr_ptr = buff + sizeof(dns_header); // curr ptr to the query buffer
    uint8_t* curr_reply_ptr = (uint8_t*) (reply_buff + (event.msg.sz - event.msg.curr_offset)); // curr ptr to the response buffer

    for (int i  = 0; i < no_queries; i++) {
        struct dns_query query;
        curr_ptr += extract_query(&query, curr_ptr, buff);

        switch (query.q_type) {
            case DNS_TYPE_A: {
                cout << "received A query for name: " << query.q_name.c_str() << endl;

                auto it = mappings_type_a.find(string((char*) query.q_name.c_str()));
                if (it == mappings_type_a.end()) {
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
                ip4_addr addr = (*it).second;
                write_in_network_order((uint8_t*) curr_ans.rdata.c_str(), &addr);

                curr_reply_ptr += write_response_to_buff(curr_reply_ptr, &curr_ans);
                break;
            }
            case DNS_TYPE_MX: {
                cout << "received MX query for name: " << query.q_name.c_str() << endl;

                auto it = mappings_type_mx.find(string((char*) query.q_name.c_str()));
                if (it == mappings_type_mx.end()) {
                    cerr << "no mapping for this name" << endl;
                    break;
                }

                struct dns_answer curr_ans;
                curr_ans.name = query.q_name;
                curr_ans.type = query.q_type;
                curr_ans._class = query.q_class;
                curr_ans.ttl = 3600; // one hour

                mx_rdata_t rdata{10, ustring((uint8_t*) (*it).second.c_str()) };
                curr_ans.rdata = mx_rdata_t::convert_to_ustring(&rdata);
                curr_ans.data_len = curr_ans.rdata.size();

//                ustring decoded = encode_dns_name((uint8_t*) (*it).second.c_str());
//
//                int data_len = 2 + // preference number
//                        decoded.size(); // mail domain len
//                curr_ans.data_len = data_len;
//
//                curr_ans.rdata = ustring (data_len, 0);
//                *(uint16_t*) curr_ans.rdata.c_str() = htons(10);
//                memcpy((uint8_t*) curr_ans.rdata.c_str() + 2, (uint8_t*) decoded.c_str(), decoded.size());

                curr_reply_ptr += write_response_to_buff(curr_reply_ptr, &curr_ans);
                break;
            }
            default: {
                cerr << "got query with unknown type" << endl;
                throw;
            }
        }

    }

    ((struct dns_header*) reply_buff)->ans_count = htons(no_queries);

    send_msg msg;
    msg.buff = reply_buff;
    msg.count = ((long long) curr_reply_ptr) - ((long long) reply_buff);

    udpServer.send_data_to_client(event.source_addr, event.source_port, msg);
}
