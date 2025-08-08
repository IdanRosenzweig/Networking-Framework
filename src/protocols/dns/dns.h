#pragma once

#include <string>
#include <vector>
#include <cstring>
using namespace std;

#include "src/abstract/net_access/net_access.h"

#define DNS_SERVER_PORT 53


// encode name such as nasa.com to 4nasa3com
vector<uint8_t> encode_dns_name(vector<uint8_t> const& name);

// decode the name in place
vector<uint8_t> decode_dns_name(vector<uint8_t> const& name);

// extract_from_network_order the encoded name at curr_ptr from the packet
int extract_encoded_name(vector<uint8_t>* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff);


// types of dns records
enum class dns_record_type : uint16_t {
    a = 1, // domain name to ipv4
    ns = 2,
    cname = 5,
    soa = 6,
    ptr = 12, // ipv4 to domain name
    mx = 15 // domain name to ipv4 of mail aggregator
};
optional<string> dns_record_type_to_str(dns_record_type type);
optional<dns_record_type> str_to_dns_record_type(string const& str);


// classes of dns records
enum dns_record_class : uint16_t {
    in = 1,
    ch = 3
};
optional<string> dns_record_class_to_str(dns_record_class _class);
optional<dns_record_class> str_to_dns_record_class(string const& str);

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
    uint16_t z: 1; // reserved
    uint16_t ra: 1; // recursion available

    uint16_t query_count; // number of query entries
    uint16_t ans_count; // number of answer entries
    uint16_t auth_count; // number of authority entries
    uint16_t add_count; // number of resource entries
};

int write_in_network_order(uint8_t* dest, struct dns_header const* source);

int extract_from_network_order(struct dns_header* dest, uint8_t const* src);


struct dns_query {
    vector<uint8_t> q_name; // stored in normal form (unencoded)
    dns_record_type q_type; // host byte order
    dns_record_class q_class; // host byte order
};

dns_query make_dns_query(string const& name, dns_record_type type, dns_record_class _class);

// write the query into dest
int write_in_network_order(uint8_t* dest, struct dns_query const* source);

// extract_from_network_order the query at curr_ptr from the packet
int extract_from_network_order(struct dns_query* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff);


typedef vector<uint8_t> rdata_t;
struct dns_answer {
    vector<uint8_t> name; // stored in normal form (unencoded)

    dns_record_type type; // host byte order
    dns_record_class _class; // host byte order
    uint32_t ttl; // host byte order
    uint16_t data_len; // host byte order

    rdata_t rdata; // stored raw (maybe encoded), can be converted to the various types' specifications if needed
};

// write the response into dest
int write_in_network_order(uint8_t* dest, struct dns_answer const* source);

// extract_from_network_order the response at curr_ptr from the packet
int extract_from_network_order(struct dns_answer* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff);


struct mx_rdata_t {
    uint16_t preference;
    vector<uint8_t> domain;
};

rdata_t convert_to_rdata(mx_rdata_t const* source);

int extract_from_network_order(mx_rdata_t* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff);


struct soa_rdata_t {
    vector<uint8_t> primary_server;
    vector<uint8_t> admin_mail_server;
    uint32_t serial_num;
    uint32_t refresh_interval;
    uint32_t retry_interval;
    uint32_t expire_limit;
    uint32_t max_ttl;
};

rdata_t convert_to_rdata(soa_rdata_t const* source);

int extract_from_network_order(soa_rdata_t* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff);



struct dns_message {
    struct dns_header header;
    vector<dns_query> queries;
    vector<dns_answer> ans;
    vector<dns_answer> ans_auth;
    vector<dns_answer> ans_addit;
};

int write_in_network_order(uint8_t* dest, struct dns_message const* src);

int extract_from_network_order(struct dns_message* dest, uint8_t const* src);


void print_dns_message(struct dns_message const* msg, bool print_queries, bool print_ans);

namespace dns_protocol {
    void send(shared_ptr<net_access> const& net_access, dns_message const& msg);

    void connect_recv(
        shared_ptr<net_access> const& net_access, shared_ptr<recv_listener<struct dns_message>> const& recv
    );
}