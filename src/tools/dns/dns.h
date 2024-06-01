#ifndef NETWORKING_DNS_H
#define NETWORKING_DNS_H

#include <string>
#include <cstring>
#include <netinet/in.h>
#include "../../abstract/utils/udata.h"
using namespace std;

#define DNS_SERVER_PORT 53


// encode name such as nasa.com to 4nasa3com
ustring encode_dns_name(const uint8_t *name);

// decode the name in place
void decode_dns_name(ustring &name);

// extract_from_network_order the encoded name at curr_ptr from the packet
int extract_encoded_name(ustring *dest, uint8_t *curr_ptr, uint8_t *packet_buff);


// types of dns records
enum dns_record_type {
    DNS_TYPE_A = 1, // domain name to ipv4
    DNS_TYPE_NS = 2,
    DNS_TYPE_CNAME = 5,
    DNS_TYPE_SOA = 6,
    DNS_TYPE_PTR = 12, // ipv4 to domain name
    DNS_TYPE_MX = 15 // domain name to ipv4 of mail aggregator
};

// classes of dns records
enum dns_record_class {
    DNS_CLASS_IN = 1
};

dns_record_type str_to_record_type(const string& str);


struct dns_header { // all fields stored in host byte order
    uint16_t id; // identification number

    // flags
    uint16_t rd: 1; // recursion desired
    uint16_t tc: 1; // truncated message
    uint16_t aa: 1; // authoritive answer
    uint16_t opcode: 4; // purpose of message
    uint16_t qr: 1; // dns_query/response flag
    uint16_t rcode: 4; // response code
    uint16_t cd: 1; // checking disabled
    uint16_t ad: 1; // authenticated plain_data
    uint16_t z: 1; // its z! reserved
    uint16_t ra: 1; // recursion available

    uint16_t query_count; // number of query entries
    uint16_t ans_count; // number of answer entries
    uint16_t auth_count; // number of authority entries
    uint16_t add_count; // number of resource entries
};

int write_in_network_order(uint8_t *dest, struct dns_header *source);

int extract_from_network_order(struct dns_header *dest, uint8_t *src);


struct dns_query {
    ustring q_name; // stored in normal form (unencoded)
    uint16_t q_type; // host byte order
    uint16_t q_class; // host byte order
};

// write the query into dest
int write_in_network_order(uint8_t *dest, struct dns_query *source);

// extract_from_network_order the query at curr_ptr from the packet
int extract_from_network_order(struct dns_query *dest, uint8_t *curr_ptr, uint8_t *packet_buff);


typedef ustring rdata_t;
struct dns_answer {
    ustring name; // stored in normal form (unencoded)

    uint16_t type; // host byte order
    uint16_t _class; // host byte order
    uint32_t ttl; // host byte order
    uint16_t data_len; // host byte order

    rdata_t rdata; // stored raw (maybe encoded), can be converted to the various types' specifications if needed
};

// write the response into dest
int write_in_network_order(uint8_t *dest, struct dns_answer *source);

// extract_from_network_order the response at curr_ptr from the packet
int extract_from_network_order(struct dns_answer *dest, uint8_t *curr_ptr, uint8_t *packet_buff);


struct mx_rdata_t {
    uint16_t preference;
    ustring domain;
};

rdata_t convert_to_rdata(mx_rdata_t *source);

int extract_from_network_order(mx_rdata_t *dest, uint8_t *curr_ptr, uint8_t *packet_buff);


struct soa_rdata_t {
    ustring primary_server;
    ustring admin_mail_server;
    uint32_t serial_num;
    uint32_t refresh_interval;
    uint32_t retry_interval;
    uint32_t expire_limit;
    uint32_t max_ttl;
};

rdata_t convert_to_rdata(soa_rdata_t *source);

int extract_from_network_order(soa_rdata_t *dest, uint8_t *curr_ptr, uint8_t *packet_buff);

#endif //NETWORKING_DNS_H
