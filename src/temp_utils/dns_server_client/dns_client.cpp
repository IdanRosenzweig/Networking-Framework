#include "dns_client.h"

#include <vector>
#include <iostream>
using namespace std;

dns_client::dns_client(ip4_addr dest_server_ip, ip4_addr src_ip, msg_gateway* network_layer_gw) : udpClient(dest_server_ip, DNS_SERVER_PORT, 1212, src_ip, network_layer_gw) {
    udpClient.add_listener(this);
}
//dns_client::dns_client(ip4_addr server_addr, msg_gateway *gw) : empClient(server_addr,{0x53}, {0x12,0x12}, gw) {
//    empClient.add_listener(this);
//}

void dns_client::query(dns_record_type type, const std::string& key) {

#define BUFF_SZ 65536
    uint8_t buff[BUFF_SZ] = {0};

    // build the dns request header
    struct dns_header header;
    header.id = 3576;
    header.qr = 0; // this is a query
    header.opcode = 0; // standard
    header.aa = 0;
    header.tc = 0;
    header.rd = 1; // don't want to start handle recursion...
    header.ra = 0;
//    header.z = 0;
//    header.ad = 0;
//    header.cd = 0;
    header.rcode = 0;
    header.query_count = 1; // the number of queries
    header.ans_count = 0;
    header.auth_count = 0;
    header.add_count = 0;
    write_in_network_order(buff, &header);

    // add queries
    int queries_sz = 0;

    struct dns_query query;
    query.q_name = ustring((unsigned char*) key.c_str(), key.size());
    query.q_type = type; // the type
    query.q_class = 1; // the class
    queries_sz = write_in_network_order(buff + sizeof(struct dns_header) + queries_sz, &query);

    // send the dns header
    cout << "requesting 1 dns record" << endl;
    int cnt = sizeof(struct dns_header) + queries_sz;
    send_msg send;
    memcpy(send.get_active_buff(), buff, cnt);
    send.set_count(cnt);
    udpClient.send_data(send);
//    empClient.send_data(send);


    // receive answer
    received_msg reply = front_data();
    uint8_t *reply_buff = reply.data.data() + reply.curr_offset;
    uint8_t *curr_ptr = reply_buff;

    struct dns_header response;
    curr_ptr += extract_from_network_order(&response, curr_ptr);

    cout << "received response" << endl;
    uint16_t no_answers = response.ans_count;
    cout << "- " << no_answers << " answers" << endl;
    uint16_t no_authoritative_answers = response.auth_count;
    cout << "- " << no_authoritative_answers << " authoritative server answers" << endl;
    uint16_t no_addit_data = response.add_count;
    cout << "- " << no_addit_data << " additional records data" << endl << endl;

    // jump over the queries we sent
    curr_ptr += queries_sz;

    // read answers
    if (no_answers > 0) {
        vector<dns_answer> answers;
        for (int i = 0; i < no_answers; i++) {
            struct dns_answer curr;
            curr_ptr += extract_from_network_order(&curr, curr_ptr, reply_buff);
            answers.push_back(std::move(curr));
        }

        cout << "# answers:" << endl;
        for (int i = 0; i < no_answers; i++) {

            cout << "Name : " << answers[i].name.c_str() << ", ";
            if (answers[i].type == DNS_TYPE_A)
            {
                struct sockaddr_in a;
                a.sin_addr.s_addr = (*(uint32_t *) (answers[i].rdata.c_str()));
                cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

            } else if (answers[i].type == DNS_TYPE_CNAME) {
                ustring decoded_name;
                extract_encoded_name(
                        &decoded_name, (uint8_t*) answers[i].rdata.c_str(), buff
                );
                decode_dns_name(decoded_name);

                cout << "has alias key : " << decoded_name.c_str();

            } else if (answers[i].type == DNS_TYPE_MX) {
                struct mx_rdata_t rdata;
                extract_from_network_order(&rdata, (uint8_t *) answers[i].rdata.c_str(), buff);
                cout << "has mail aggregator : " << rdata.domain.c_str();
            }

            cout << endl;
        }

        cout << endl;
    }

    // read authorities
    if (no_authoritative_answers > 0) {
        vector<dns_answer> auth;
        for (int i = 0; i < no_authoritative_answers; i++) {
            struct dns_answer curr;
            curr_ptr += extract_from_network_order(&curr, curr_ptr, reply_buff);
            auth.push_back(std::move(curr));
        }

        cout << "# authoritative server answers:" << endl;
        for (int i = 0; i < no_authoritative_answers; i++) {

            if (auth[i].type == DNS_TYPE_NS) {
                cout << "has nameserver: " << auth[i].rdata.c_str();
            } else if (auth[i].type == DNS_TYPE_SOA) {
                struct soa_rdata_t rdata;
                extract_from_network_order(&rdata, (uint8_t *) auth[i].rdata.c_str(), buff);
                cout << "start of authority:" << endl;
                cout << "\t" << "primary server: " << rdata.primary_server.c_str() << endl;
                cout << "\t" << "responsible mailbox: " << rdata.admin_mail_server.c_str() << endl;
            }
        }

        cout << endl;
    }

    // read additional
    if (no_addit_data > 0) {
        vector<dns_answer> addit;
        for (int i = 0; i < no_addit_data; i++) {
            struct dns_answer curr;
            curr_ptr += extract_from_network_order(&curr, curr_ptr, reply_buff);
            addit.push_back(std::move(curr));
        }

        cout << "# additional records:" << endl;
        for (int i = 0; i < no_addit_data; i++) {

            cout << "Name : " << addit[i].name.c_str() << ", ";
            if ((addit[i].type) == DNS_TYPE_A) {
                struct sockaddr_in a;
                a.sin_addr.s_addr = (*(uint32_t *) (addit[i].rdata.c_str()));
                cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

            }

            cout << endl;
        }

        cout << endl;
    }

}

