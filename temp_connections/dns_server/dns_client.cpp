#include "dns_client.h"

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <vector>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

void dns_client::query(const string &name) {

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
    query.q_name = ustring((unsigned char*) name.c_str());
    query.q_type = DNS_TYPE_MX; // the type
    query.q_class = 1; // the class

    queries_sz += write_query_to_buff(((uint8_t *) buff) + sizeof(struct dns_header) + queries_sz, &query);


    // send the dns request
    gateway->send_data({(char *) buff, (int) sizeof(struct dns_header) + queries_sz});


    // receive answer
    received_msg reply = front_data();
    uint8_t *reply_buff = reply.data.get() + reply.curr_offset;


    struct dns_header *response = (struct dns_header *) reply_buff;

    cout << "response for " << name.c_str() << " contains : " << endl;
    cout << ntohs(response->query_count) << " questions were sent." << endl;
    cout << ntohs(response->ans_count) << " Answers." << endl;
    cout << ntohs(response->auth_count) << " Authoritative Servers." << endl;
    cout << ntohs(response->add_count) << " Additional records." << endl << endl;

    // jump over the header and the query
    uint8_t *curr_ptr = &reply_buff[sizeof(struct dns_header) + queries_sz];


    // read answers
    vector<dns_answer> answers;
    for (int i = 0; i < ntohs(response->ans_count); i++) {
        struct dns_answer curr;

        curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

        answers.push_back(std::move(curr));
    }

    // read authorities
    vector<dns_answer> auth;
    for (int i = 0; i < ntohs(response->auth_count); i++) {
        struct dns_answer curr;

        curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

        auth.push_back(std::move(curr));
    }

    // read additional
    vector<dns_answer> addit;
    for (int i = 0; i < ntohs(response->add_count); i++) {
        struct dns_answer curr;

        curr_ptr += extract_response(&curr, curr_ptr, reply_buff);

        addit.push_back(std::move(curr));
    }

    // print answers
    cout << "Answer Records : " << ntohs(response->ans_count) << endl;
    for (int i = 0; i < ntohs(response->ans_count); i++) {

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

            cout << "has alias name : " << decoded_name.c_str();

        } else if (answers[i].type == DNS_TYPE_MX) {
            struct mx_rdata_t rdata;
            mx_rdata_t::extract(&rdata, (uint8_t*) answers[i].rdata.c_str(), buff);
            cout << "has mail server : " << rdata.domain.c_str();
        }

        cout << endl;
    }

    // print authorities
    cout << endl << "Authoritive Records : " << ntohs(response->auth_count) << endl;
    for (int i = 0; i < ntohs(response->auth_count); i++) {

        cout << "Name : " << auth[i].name.c_str() << ", ";
        if ((auth[i].type) == DNS_TYPE_NS) {
            cout << "has nameserver : " << auth[i].rdata.c_str();
        }

        cout << endl;
    }

    // print additional msg
    cout << endl << "Additional Records : " << ntohs(response->add_count) << endl;
    for (int i = 0; i < ntohs(response->add_count); i++) {

        cout << "Name : " << addit[i].name.c_str() << ", ";
        if ((addit[i].type) == DNS_TYPE_A) {
            struct sockaddr_in a;
            a.sin_addr.s_addr = (*(uint32_t *) (addit[i].rdata.c_str()));
            cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

        }

        cout << endl;
    }

}
