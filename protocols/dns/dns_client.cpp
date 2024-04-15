#include "dns_client.h"

#include "dns_types.h"

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <vector>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

string encode_name(unsigned char *name) {
//    strcat((char *) name, ".");
//    int lock = 0;
//    for (int i = 0; i < strlen((char *) name); i++) {
//        if (name[i] == '.') {
//            *encoded++ = i - lock;
//            for (; lock < i; lock++) {
//                *encoded++ = name[lock];
//            }
//            lock++; //or lock=i+1;
//        }
//    }
//    *encoded++ = '\0';

    string encoded;

    int name_len = strlen((char *) name);
    int last = 0;
    int curr = 0;
    while (curr < name_len) {
        if (name[curr] == '.') {
            encoded += curr - last;

            for (int j = last; j < curr; j++) {
                encoded += name[j];
            }

            last = curr + 1;
        }

        curr++;
    }

    encoded += curr - last;
    for (int i = last; i < name_len; i++) {
        encoded += name[i];
    }

    return encoded;
}

// returns the decodes name, puts in count the number of bytes advanced after curr_ptr
string decode_name(unsigned char *reader, unsigned char *buffer, int *count) {
    string name;
    *count = 0;

    // extract the decoded name from the buffer
    bool jumped = false;
    while (*reader != 0) {
        if (*reader >= 192) { // there is a jump
            unsigned offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset;

            jumped = true; // marking we have jumped

        } else {
            name += *reader;
            reader += +1;

            if (!jumped) // we are not after a jump, increment the count
                *count = *count + 1;
//            else // we are after a jump, this byte is not stored in our current answer space
        }
    }

    // update the final value of count
    if (jumped) // string ended after we jumped, meaning additional 2 bytes for the offset used in the jump
        *count += 2;
    else // string ended without a jump, add 1 byte for the trailing null byte
        *count += 1;

    // now decode the decoded name
    int i = 0;
    while (i < name.size()) {
        int cnt = name[i];

        int j = 0;
        while (j < cnt) {
            name[i] = name[i + 1];
            j++;
            i++;
        }
        name[i] = '.';

        i++;
    }
    if (name[name.size() - 1] == '.')
        name.pop_back();

    return name;
}
//string decode_name(unsigned char *reader, unsigned char *buffer, int *count) {
//    bool jumped = false;
//
//    *count = 1;
//
//    string name;
//
//    // extract the decoded name from the buffer
//    while (*reader != 0) {
//        if (*reader >= 192) {
//            unsigned offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
//            reader = buffer + offset - 1;
//            jumped = true; //we have jumped to another location so counting wont go up!
//        } else {
//            name += *reader;
//        }
//
//        reader = reader + 1;
//
//        if (!jumped) {
//            *count = *count + 1; //if we havent jumped to another location then we can count up
//        }
//    }
//
//    if (jumped) {
//        *count = *count + 1; //number of steps we actually moved forward in the packet
//    }
//
//    //now convert 3www6google3com0 to www.google.com
//    int i;
//    for (i = 0; i < (int) name.size(); i++) {
//        int cnt = name[i];
//        for (int j = 0; j < (int) cnt; j++) {
//            name[i] = name[i + 1];
//            i = i + 1;
//        }
//        name[i] = '.';
//    }
//    name[i - 1] = '\0'; //remove the last dot
//    return name;
//}

void dns_client::query(const std::string &name) {
    // total buffer
#define BUFF_SZ 65536
    unsigned char buf[BUFF_SZ];
    memset(buf, '\x00', sizeof(BUFF_SZ));

    // build the dns request header
    struct dns_header *request = (struct dns_header *) &buf;
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
    request->q_count = htons(1); //we have only 1 question
    request->ans_count = 0;
    request->auth_count = 0;
    request->add_count = 0;

    // add one dns_query
    struct dns_query query;
    query.q_name = encode_name((unsigned char *) name.c_str());
    query.q_type = htons(T_A); // write the dns_query type
    query.q_class = htons(1); // write the dns_query class

    query.write_to_buff((char *) (&buf[sizeof(struct dns_header)]));


    // send dns_query
//    this->udp_conn_client::send_data((char *) buf, sizeof(struct dns_header) + query.size());
//    client.send_encapsulated_data((char *) buf, sizeof(struct dns_header) + query.size());
    gateway->send_data((char *) buf, sizeof(struct dns_header) + query.size());

    // receive answer
//    this->udp_conn_client::recv_data(buf, BUFF_SZ);
//    client.recv_encapsulated_data(buf, BUFF_SZ);
//    gateway->recv_data(buf, BUFF_SZ);
    auto reply = std::move(recv_data());
    uint8_t *reply_buff = reply.msg.data.get() + reply.curr_offset;


    struct dns_header *response = (struct dns_header *) reply_buff;

    cout << "The response contains : " << endl;
    cout << ntohs(response->q_count) << " questions were sent." << endl;
    cout << ntohs(response->ans_count) << " Answers." << endl;
    cout << ntohs(response->auth_count) << " Authoritative Servers." << endl;
    cout << ntohs(response->add_count) << " Additional records." << endl << endl;

    // jump over the header and the dns_query
    uint8_t *reader = &reply_buff[sizeof(struct dns_header) + query.size()];


    // read answers
    vector<dns_answer> answers;
    unique_ptr<int> curr = make_unique<int>();
    for (int i = 0; i < ntohs(response->ans_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader,
                                reply_buff, &advanced);
        reader += advanced;

        curr.read_const_sz_fields(reader);
        reader += curr.sizeof_const_sz_fields();

        if (ntohs(curr.type) == T_A) // answer gave ip address
        {
            for (int j = 0; j < ntohs(curr.data_len); j++)
                curr.rdata += reader[j];

            reader += ntohs(curr.data_len);

        } else if (ntohs(curr.type) == T_CNAME) {
            curr.rdata = decode_name(reader, reply_buff, &advanced);
            reader += advanced;

        } else {
            perror("not supported type of record\"");
            throw;
        }


        answers.push_back(std::move(curr));
    }

    // read authorities
    vector<dns_answer> auth;
    for (int i = 0; i < ntohs(response->auth_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader, reply_buff, &advanced);
        reader += advanced;

        answers[i].read_const_sz_fields(reader);
        reader += answers[i].sizeof_const_sz_fields();

        curr.rdata = decode_name(reader, reply_buff, &advanced);
        reader += advanced;

        answers.push_back(std::move(curr));
    }

    // read additional
    vector<dns_answer> addit;
    for (int i = 0; i < ntohs(response->add_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader, reply_buff, &advanced);
        reader += advanced;

        answers[i].read_const_sz_fields(reader);
        reader += answers[i].sizeof_const_sz_fields();

        if (ntohs(curr.type) == T_A) { // answer gave ip address
            for (int j = 0; j < ntohs(curr.data_len); j++)
                curr.rdata += reader[j];

            reader += ntohs(curr.data_len);

        } else if (ntohs(curr.type) == T_CNAME) {
            curr.rdata = decode_name(reader, reply_buff, &advanced);
            reader += advanced;

        } else {
            perror("not supported type of record\"");
            throw;
        }

        answers.push_back(std::move(curr));
    }

    // print answers
    cout << "Answer Records : " << ntohs(response->ans_count) << endl;
    for (int i = 0; i < ntohs(response->ans_count); i++) {

        printf("Name : %s ", answers[i].name.c_str());
        if (ntohs(answers[i].type) == T_A) // answer gave ip address
        {
            struct sockaddr_in a;
            a.sin_addr.s_addr = (*(uint32_t *) (answers[i].rdata.c_str())); //working without ntohl
            cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

        } else if (ntohs(answers[i].type) == T_CNAME) {
            //Canonical name for an alias
            cout << "has alias name : %s" << answers[i].rdata.c_str();

        }

        cout << endl;
    }

    // print authorities
    cout << endl << "Authoritive Records : " << ntohs(response->auth_count) << endl;
    for (int i = 0; i < ntohs(response->auth_count); i++) {

        cout << "Name : " << auth[i].name.c_str();
        if (ntohs(auth[i].type) == T_NS) {
            cout << "has nameserver : " << auth[i].rdata.c_str();
        }

        cout << endl;
    }

    // print additional data
    cout << endl << "Additional Records : " << ntohs(response->add_count) << endl;
    for (int i = 0; i < ntohs(response->add_count); i++) {

        cout << "Name : " << addit[i].name.c_str();
        if (ntohs(addit[i].type) == T_A) {
            struct sockaddr_in a;
            a.sin_addr.s_addr = (*(uint32_t *) (addit[i].rdata.c_str()));
            cout << "has IPv4 address : " << inet_ntoa(a.sin_addr);

        }

        cout << endl;
    }

}

