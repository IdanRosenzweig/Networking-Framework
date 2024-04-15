#ifndef SERVERCLIENT_DNS_TYPES_H
#define SERVERCLIENT_DNS_TYPES_H

#include <stdint.h>
#include <string>
#include <cstring>

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
    std::string q_name;
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

    std::string name;

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

    std::string rdata;


};

#endif //SERVERCLIENT_DNS_TYPES_H
