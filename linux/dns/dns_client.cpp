#include "dns_client.h"

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <vector>


//string dns_client::query(const string &name) {
//
//#define LEN 200
//    unsigned char buf[LEN];
//    ns_msg msg;
//    ns_rr rr;
//
//    int response_size = res_query(name.c_str(), ns_c_in, ns_t_a, buf, sizeof(buf));
//    if (response_size < 0) {
//        perror("DNS dns_query failed");
//        return "";
//    }
//
//    if (ns_initparse(buf, response_size, &msg) < 0) {
//        perror("Error initializing message parser");
//        return "";
//    }
//
//    int rr_count = ns_msg_count(msg, ns_s_an);
//    printf("Found %d records for %s\n", rr_count, name.c_str());
//
//    for (int i = 0; i < rr_count; i++) {
//        if (ns_parserr(&msg, ns_s_an, i, &rr) < 0) {
//            perror("Error parsing resource record");
//            return "";
//        }
//        if (ns_rr_type(rr) == ns_t_a) {
//            char ip_addr[INET_ADDRSTRLEN];
//            inet_ntop(AF_INET, ns_rr_rdata(rr), ip_addr, INET_ADDRSTRLEN);
//            printf("Address: %s\n", ip_addr);
//        }
//    }
//
//    return "";
//}


struct dns_header {
    uint16_t id; // identification number

    // flags
    uint16_t rd: 1; // recursion desired
    uint16_t tc: 1; // truncated message
    uint16_t aa: 1; // authoritive answer
    uint16_t opcode: 4; // purpose of message
    uint16_t qr: 1; // dns_query/response flag
    uint16_t rcode: 4; // response code
    uint16_t cd: 1; // checking disabled
    uint16_t ad: 1; // authenticated data
    uint16_t z: 1; // its z! reserved
    uint16_t ra: 1; // recursion available

    uint16_t q_count; // number of question entries
    uint16_t ans_count; // number of answer entries
    uint16_t auth_count; // number of authority entries
    uint16_t add_count; // number of resource entries
};

// types of dns records
#define T_A 1 // ipv4 addr
#define T_NS 2
#define T_CNAME 5
#define T_SOA 6
#define T_PTR 12
#define T_MX 15 // mail server

struct dns_query {
    string q_name;
    uint16_t q_type;
    uint16_t q_class;

    void write_to_buff(char *buff) {
        memcpy(buff, q_name.c_str(), q_name.size() + 1); // including the trailing null byte
        buff += q_name.size() + 1;

        memcpy(buff, &q_type, sizeof(q_type));
        buff += sizeof(q_type);

        memcpy(buff, &q_class, sizeof(q_class));
    }

    inline int size() {
        return q_name.size() + 1 + sizeof(q_type) + sizeof(q_class);
    }
};

struct dns_answer {
    string name;

    uint16_t type;
    uint16_t _class;
    uint32_t ttl;
    uint16_t data_len;

    void read_const_sz_fields(unsigned char *buff) {
        memcpy(&type, buff, sizeof(type));
        buff += sizeof(type);

        memcpy(&_class, buff, sizeof(_class));
        buff += sizeof(_class);

        memcpy(&ttl, buff, sizeof(ttl));
        buff += sizeof(ttl);

        memcpy(&data_len, buff, sizeof(data_len));
    }

    inline int sizeof_const_sz_fields() {
        return sizeof(type) + sizeof(_class) + sizeof(ttl) + sizeof(data_len);
    }

    string rdata;


};


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
            reader += + 1;

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
    request->id = (unsigned short) htons(getpid());
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
//    udp_client.send_encapsulated_data((char *) buf, sizeof(struct dns_header) + query.size());
    udp_client->send_data((char *) buf, sizeof(struct dns_header) + query.size());

    // receive answer
//    this->udp_conn_client::recv_data(buf, BUFF_SZ);
//    udp_client.recv_encapsulated_data(buf, BUFF_SZ);
    udp_client->recv_data(buf, BUFF_SZ);


    struct dns_header *response = (struct dns_header *) buf;

    printf("\nThe response contains : ");
    printf("\n %d questions were sent.", ntohs(response->q_count));
    printf("\n %d Answers.", ntohs(response->ans_count));
    printf("\n %d Authoritative Servers.", ntohs(response->auth_count));
    printf("\n %d Additional records.\n\n", ntohs(response->add_count));

    // jump over the header and the dns_query
    unsigned char *reader = &buf[sizeof(struct dns_header) + query.size()];


    // read answers
    vector<struct dns_answer> answers;
    for (int i = 0; i < ntohs(response->ans_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader, buf, &advanced);
        reader += advanced;

        curr.read_const_sz_fields(reader);
        reader += curr.sizeof_const_sz_fields();

        if (ntohs(curr.type) == T_A) // answer gave ip address
        {
            for (int j = 0; j < ntohs(curr.data_len); j++)
                curr.rdata += reader[j];

            reader += ntohs(curr.data_len);

        } else if (ntohs(curr.type) == T_CNAME) {
            curr.rdata = decode_name(reader, buf, &advanced);
            reader += advanced;

        } else throw "not supported type of record";

        answers.push_back(curr);
    }

    // read authorities
    vector<struct dns_answer> auth;
    for (int i = 0; i < ntohs(response->auth_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader, buf, &advanced);
        reader += advanced;

        answers[i].read_const_sz_fields(reader);
        reader += answers[i].sizeof_const_sz_fields();

        curr.rdata = decode_name(reader, buf, &advanced);
        reader += advanced;

        auth.push_back(curr);
    }

    // read additional
    vector<struct dns_answer> addit;
    for (int i = 0; i < ntohs(response->add_count); i++) {
        struct dns_answer curr;

        int advanced;
        curr.name = decode_name(reader, buf, &advanced);
        reader += advanced;

        answers[i].read_const_sz_fields(reader);
        reader += answers[i].sizeof_const_sz_fields();

        if (ntohs(curr.type) == T_A) { // answer gave ip address
            for (int j = 0; j < ntohs(curr.data_len); j++)
                curr.rdata += reader[j];

            reader += ntohs(curr.data_len);

        } else if (ntohs(curr.type) == T_CNAME) {
            curr.rdata = decode_name(reader, buf, &advanced);
            reader += advanced;

        } else throw "not supported type of record";

        addit.push_back(curr);
    }

    // print answers
    printf("\nAnswer Records : %d \n", ntohs(response->ans_count));
    for (int i = 0; i < ntohs(response->ans_count); i++) {

        printf("Name : %s ", answers[i].name.c_str());
        if (ntohs(answers[i].type) == T_A) // answer gave ip address
        {
            struct sockaddr_in a;
            a.sin_addr.s_addr = (*(uint32_t*)(answers[i].rdata.c_str())); //working without ntohl
            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));

        } else if (ntohs(answers[i].type) == T_CNAME) {
            //Canonical name for an alias
            printf("has alias name : %s", answers[i].rdata.c_str());

        }

        printf("\n");
    }

    // print authorities
    printf("\nAuthoritive Records : %d \n", ntohs(response->auth_count));
    for (int i = 0; i < ntohs(response->auth_count); i++) {

        printf("Name : %s ", auth[i].name.c_str());
        if (ntohs(auth[i].type) == T_NS) {
            printf("has nameserver : %s", auth[i].rdata.c_str());
        }

        printf("\n");
    }

    // print additional data
    printf("\nAdditional Records : %d \n", ntohs(response->add_count));
    for (int i = 0; i < ntohs(response->add_count); i++) {

        printf("Name : %s ", addit[i].name.c_str());
        if (ntohs(addit[i].type) == T_A) {
            struct sockaddr_in a;
            a.sin_addr.s_addr = (*(uint32_t*)(addit[i].rdata.c_str()));
            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));

        }

        printf("\n");
    }

}

void dns_client::init() {
//    udp_client->init();
}

void dns_client::finish() {
//    udp_client->finish();
}
