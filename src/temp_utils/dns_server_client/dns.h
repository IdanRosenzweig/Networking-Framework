#ifndef SERVERCLIENT_DNS_H
#define SERVERCLIENT_DNS_H

#include <string>
#include <cstring>
#include <netinet/in.h>
using namespace std;

#define DNS_SERVER_PORT 53

typedef std::basic_string<uint8_t> ustring;

// encode name such as www.google.com to 3www6google3com
ustring encode_dns_name(unsigned char *name);

// decode the name in place
void decode_dns_name(ustring &name);

// extract the encoded name at curr_ptr from the packet
int extract_encoded_name(ustring *dest, uint8_t *curr_ptr, uint8_t *packet_buff);

// extract the query at curr_ptr from the packet
int extract_query(struct dns_query *dest, uint8_t *curr_ptr, uint8_t *packet_buff);

// extract the response at curr_ptr from the packet
int extract_response(struct dns_answer *dest, uint8_t *curr_ptr, uint8_t *packet_buff);

// write the query into dest
int write_query_to_buff(uint8_t *dest, struct dns_query *source);

int write_response_to_buff(uint8_t *dest, struct dns_answer *source);


struct dns_header { // all fields stored in network byte order
    uint16_t id; // identification number

    // flags
    uint16_t rd: 1; // recursion desired
    uint16_t tc: 1; // truncated message
    uint16_t aa: 1; // authoritive answer
    uint16_t opcode: 4; // purpose of message
    uint16_t qr: 1; // dns_query/response flag
    uint16_t rcode: 4; // response code
    uint16_t cd: 1; // checking disabled
    uint16_t ad: 1; // authenticated msg
    uint16_t z: 1; // its z! reserved
    uint16_t ra: 1; // recursion available

    uint16_t query_count; // number of query entries
    uint16_t ans_count; // number of answer entries
    uint16_t auth_count; // number of authority entries
    uint16_t add_count; // number of resource entries
};

// types of dns records
enum dns_record_type {
    DNS_TYPE_A = 1, // domain name to ipv4
    DNS_TYPE_NS = 2,
    DNS_TYPE_CNAME = 5,
    DNS_TYPE_SOA = 6,
    DNS_TYPE_PTR = 12, // ipv4 to domain name
    DNS_TYPE_MX = 15 // domain name to ipv4 of mail aggregator
};
dns_record_type str_to_record_type(const string& str);

struct dns_query {
    ustring q_name; // stored in normal form (unencoded)
    uint16_t q_type; // host byte order
    uint16_t q_class; // host byte order
};


struct dns_answer {
    ustring name; // stored in normal form (unencoded)

    uint16_t type; // host byte order
    uint16_t _class; // host byte order
    uint32_t ttl; // host byte order
    uint16_t data_len; // host byte order

    ustring rdata; // stored raw (maybe encoded), can be converted to the various types' specifications if needed
};

struct mx_rdata_t {
    uint16_t preference;
    ustring domain;

    static int extract(mx_rdata_t *dest, uint8_t *curr_ptr, uint8_t *packet_buff) {
        int cnt = 0;

        dest->preference = ntohs(*(uint16_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->preference);

        cnt += extract_encoded_name(&dest->domain, curr_ptr + cnt, packet_buff);
        decode_dns_name(dest->domain);

        return cnt;
    }

    static ustring convert_to_ustring(mx_rdata_t *source) {
        uint16_t pref = htons(source->preference);
        ustring encoded_name = encode_dns_name((uint8_t *) source->domain.c_str());

        ustring res(sizeof(pref) + encoded_name.size(), 0);

        *(uint16_t *) res.c_str() = pref;
        memcpy((uint8_t *) (res.c_str() + sizeof(pref)),
               encoded_name.c_str(), encoded_name.size());

        return res;
    }
};

struct soa_rdata_t {
    ustring primary_server;
    ustring admin_mail_server;
    uint32_t serial_num;
    uint32_t refresh_interval;
    uint32_t retry_interval;
    uint32_t expire_limit;
    uint32_t max_ttl;

    static int extract(soa_rdata_t *dest, uint8_t *curr_ptr, uint8_t *packet_buff) {
        int cnt = 0;

        cnt += extract_encoded_name(&dest->primary_server, curr_ptr + cnt, packet_buff);
        decode_dns_name(dest->primary_server);

        cnt += extract_encoded_name(&dest->admin_mail_server, curr_ptr + cnt, packet_buff);
        decode_dns_name(dest->admin_mail_server);

        dest->serial_num = ntohs(*(uint32_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->serial_num);

        dest->refresh_interval = ntohs(*(uint32_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->refresh_interval);

        dest->retry_interval = ntohs(*(uint32_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->retry_interval);

        dest->expire_limit = ntohs(*(uint32_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->expire_limit);

        dest->max_ttl = ntohs(*(uint32_t *) (curr_ptr + cnt));
        cnt += sizeof(dest->max_ttl);

        return cnt;
    }

//    static int write_to_buff(soa_rdata_t* dest, uint8_t* curr_ptr, uint8_t* packet_buff) {
//
//
//    }
};


#endif //SERVERCLIENT_DNS_H
