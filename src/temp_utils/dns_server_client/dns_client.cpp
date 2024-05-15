#include "dns_client.h"

#include <vector>
#include <iostream>
using namespace std;

#include "../../linux/hardware.h"

dns_client::dns_client(ip4_addr server_addr, msg_gateway *gw) {
    if (gw == nullptr) gateway = new network_layer_gateway("enp0s3");
    else gateway = gw;

    // setup send flow
    ip_client.gateway = gateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(server_addr);
    ip_client.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));
//        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));

    udp_client.gateway = &ip_client;
    udp_client.next_source_port.set_next_choice(1212);
    udp_client.next_dest_port.set_next_choice(DNS_SERVER_PORT);

    // setup recv flow
    gateway->add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

    udp_client.port_handlers.assign_to_key(1212, this);
}

void dns_client::query(dns_record_type type, const std::string& key) {

#define BUFF_SZ 65536
    unsigned char buff[BUFF_SZ];
    memset(buff, '\x00', BUFF_SZ);

    // build the dns request header
    struct dns_header *request = (struct dns_header *) &buff;
    request->id = (unsigned short) htons(3576);
    request->qr = 0; //This is a dns_query
    request->opcode = 0; //This is a standard dns_query
    request->aa = 0; //Not Authoritative
    request->tc = 0; //This message is not truncated
    request->rd = 1; //Recursion Desired
    request->ra = 0; //Recursion not available! hey we dont have it (lol)
    request->z = 0;
    request->ad = 0;
    request->cd = 0;
    request->rcode = 0;
    request->query_count = htons(1); // the number of queries
    request->ans_count = 0;
    request->auth_count = 0;
    request->add_count = 0;


    int queries_sz = 0;

    // add one query
    struct dns_query query;
    query.q_name = ustring((unsigned char*) key.c_str(), key.size());
    query.q_type = type; // the type
    query.q_class = 1; // the class

    queries_sz += write_query_to_buff(((uint8_t *) buff) + sizeof(struct dns_header) + queries_sz, &query);


    // send the dns request
    cout << "requesting 1 dns record" << endl;
    send_msg send{(char *) buff, (int) sizeof(struct dns_header) + queries_sz};
    udp_client.send_data(send);


    // receive answer
    received_msg reply = front_data();
    uint8_t *reply_buff = reply.data.get() + reply.curr_offset;


    struct dns_header *response = (struct dns_header *) reply_buff;

    cout << "received response" << endl;
    int no_answers = ntohs(response->ans_count);
    int no_authoritative_answers = ntohs(response->auth_count);
    int no_addit_data = ntohs(response->add_count);
    cout << "- " << no_answers << " answers" << endl;
    cout << "- " << no_authoritative_answers << " authoritative server answers" << endl;
    cout << "- " << no_addit_data << " additional records data" << endl << endl;

    // jump over the header and the query
    uint8_t *curr_ptr = &reply_buff[sizeof(struct dns_header) + queries_sz];

    // read answers
    if (no_answers > 0) {
        vector<dns_answer> answers;
        for (int i = 0; i < no_answers; i++) {
            struct dns_answer curr;

            curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

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
                mx_rdata_t::extract(&rdata, (uint8_t*) answers[i].rdata.c_str(), buff);
                cout << "has mail aggregator : " << rdata.domain.c_str();
            }

            cout << endl;
        }
    }

    // read authorities
    if (no_authoritative_answers > 0) {
        vector<dns_answer> auth;
        for (int i = 0; i < no_authoritative_answers; i++) {
            struct dns_answer curr;

            curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

            auth.push_back(std::move(curr));
        }

        cout << "# authoritative server answers:" << endl;
        for (int i = 0; i < no_authoritative_answers; i++) {

            cout << "Name : " << auth[i].name.c_str() << ", ";
            if ((auth[i].type) == DNS_TYPE_NS) {
                cout << "has nameserver : " << auth[i].rdata.c_str();
            }

            cout << endl;
        }
    }

    // read additional
    if (no_addit_data > 0) {
        vector<dns_answer> addit;
        for (int i = 0; i < no_addit_data; i++) {
            struct dns_answer curr;

            curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

            addit.push_back(std::move(curr));
        }

        cout << "# additional record data:" << endl;
        for (int i = 0; i < no_addit_data; i++) {

            cout << "Name : " << addit[i].name.c_str() << ", ";
            if ((addit[i].type) == DNS_TYPE_A) {
                struct sockaddr_in a;
                a.sin_addr.s_addr = (*(uint32_t *) (addit[i].rdata.c_str()));
                cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

            }

            cout << endl;
        }
    }

}

