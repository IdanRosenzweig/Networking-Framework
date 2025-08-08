#include "dns.h"

#include <netinet/in.h>

#include <iostream>

#include "src/protocols/ip4/ip4_addr.h"

#include "lib/hexdump.h"

vector<uint8_t> encode_dns_name(vector<uint8_t> const& data) {
    vector<uint8_t> encoded;

    int last = 0;
    int curr = 0;
    while (curr < data.size()) {
        if (data[curr] == '.' || (curr == data.size() - 1)) {
            int len = curr - last;
            encoded.push_back(len);

            for (int j = last; j < curr; j++) {
                encoded.push_back(data[j]);
            }

            curr++;
            last = curr;

        } else
            curr++;
    }

    encoded.push_back(0); // ending with null byte

    return encoded;
}

vector<uint8_t> decode_dns_name(vector<uint8_t> const& name) {
    vector<uint8_t> res;

    int i = 0;
    while (i < name.size()) {
        int len = name[i];
        if (len == 0) {
            res.push_back(0);
            break;
        }

        if (!res.empty()) res.push_back('.');

        ++i;
        int j = 0;
        while (j < len) {
            res.push_back(name[i]);
            ++i;
            ++j;
        }
    }

    return res;
}

int extract_encoded_name(vector<uint8_t>* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff) {
    int count = 0;

    // extract_from_network_order the encoded name from the packet
    bool jumped = false;
    while (*curr_ptr != 0) {

#define JUMP_MASK (0b11000000)
        if ((*curr_ptr & JUMP_MASK) == JUMP_MASK) { // this is a jump

            int offset = (((*curr_ptr) ^ JUMP_MASK) << 8) + *(curr_ptr + 1); // the value (number is in big endian)

            curr_ptr = packet_buff + offset;
            jumped = true;

        } else {
            dest->push_back(*curr_ptr);
            curr_ptr++;

            if (!jumped) // we are not after a jump, next_item the count
                count++;
//            else // we are after a jump, this byte is not stored in our current answer space
        }
    }
    dest->push_back(0); // null byte

    // update the final value of count
    if (jumped) // string ended AFTER we jumped, meaning additional 2 bytes for the offset used in the jump
        count += 2;
    else // string ended without a jump, add 1 byte for the trailing null byte
        count += 1;

    return count;
}

optional<string> dns_record_type_to_str(dns_record_type type) {
    switch (type) {
        case dns_record_type::a: return "a";
        case dns_record_type::mx: return "mx";
        case dns_record_type::ptr: return "ptr";
        case dns_record_type::cname: return "cname";
        case dns_record_type::ns: return "ns";
        case dns_record_type::soa: return "soa";
        default: return {};
    }
}

optional<dns_record_type> str_to_dns_record_type(string const& str) {
    if (str == "a") return dns_record_type::a;
    else if (str == "mx") return dns_record_type::mx;
    else if (str == "ptr") return dns_record_type::ptr;
    else if (str == "cname") return dns_record_type::cname;
    else if (str == "ns") return dns_record_type::ns;
    else if (str == "soa") return dns_record_type::soa;
    else return {};
}

optional<string> dns_record_class_to_str(dns_record_class _class) {
    switch (_class) {
        case dns_record_class::in: return "in";
        case dns_record_class::ch: return "ch";
        default: return {};
    }
}

optional<dns_record_class> str_to_dns_record_class(string const& str) {
    if (str == "in") return dns_record_class::in;
    else if (str == "ch") return dns_record_class::ch;
    else return {};
}


int write_in_network_order(uint8_t* dest, struct dns_header const* source) {
    uint8_t * curr = dest;

    *(uint16_t*) curr = htons(source->id);
    curr += sizeof(uint16_t);

    *curr = source->rd | (source->tc << 1) | (source->aa << 2) << (source->opcode << 3) | (source->qr << 7);
    curr++;
    *curr =source->rcode | (source->cd << 4) | (source->ad << 5) << (source->ra << 7);
    curr++;

    *(uint16_t*) curr = htons(source->query_count);
    curr += sizeof(uint16_t);
    *(uint16_t*) curr = htons(source->ans_count);
    curr += sizeof(uint16_t);
    *(uint16_t*) curr = htons(source->auth_count);
    curr += sizeof(uint16_t);
    *(uint16_t*) curr = htons(source->add_count);
    curr += sizeof(uint16_t);

    return curr - dest;
}

int extract_from_network_order(struct dns_header* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    dest->id = ntohs(*(uint16_t*) curr);
    curr += sizeof(uint16_t);

    dest->qr = ((*curr) & (0b1 << 7)) >> 7;
    dest->opcode = ((*curr) & (0b1111 << 3)) >> 3;
    dest->aa = ((*curr) & (0b1 << 2)) >> 2;
    dest->tc = ((*curr) & (0b1 << 1)) >> 1;
    dest->rd = (*curr) & 0b1;
    curr++;

    dest->ra = ((*curr) & (0b1 << 7)) >> 7;
    dest->ad = ((*curr) & (0b1 << 5)) >> 5;
    dest->cd = ((*curr) & (0b1 << 4)) >> 4;
    dest->rcode = (*curr) & 0b1111;
    curr++;

    dest->query_count = ntohs(*(uint16_t*) curr);
    curr += sizeof(uint16_t);
    dest->ans_count = ntohs(*(uint16_t*) curr);
    curr += sizeof(uint16_t);
    dest->auth_count = ntohs(*(uint16_t*) curr);
    curr += sizeof(uint16_t);
    dest->add_count = ntohs(*(uint16_t*) curr);
    curr += sizeof(uint16_t);

    return curr - src;
}


dns_query make_dns_query(string const& name, dns_record_type type, dns_record_class _class) {
    return dns_query{
        vector<uint8_t>(name.data(), name.data() + name.size() + 1),
        type,
        _class
    };
}

int write_in_network_order(uint8_t* dest, struct dns_query const* source) {
    int cnt = 0;

    vector<uint8_t> encoded_q_name = encode_dns_name(source->q_name);
    memcpy(dest + cnt, encoded_q_name.data(), encoded_q_name.size());
    cnt += encoded_q_name.size();

    *(uint16_t *) (dest + cnt) = htons(static_cast<uint16_t>(source->q_type));
    cnt += sizeof(source->q_type);

    *(uint16_t *) (dest + cnt) = htons(static_cast<uint16_t>(source->q_class));
    cnt += sizeof(source->q_class);

    return cnt;
}

int extract_from_network_order(struct dns_query *dest, uint8_t const* curr_ptr, uint8_t const* packet_buff) {
    int cnt = 0;

    cnt += extract_encoded_name(&dest->q_name, curr_ptr + cnt, packet_buff);
    dest->q_name = decode_dns_name(dest->q_name);

    dest->q_type = static_cast<dns_record_type>(ntohs(*(uint16_t *) (curr_ptr + cnt)));
    cnt += sizeof(dest->q_type);

    dest->q_class = static_cast<dns_record_class>(ntohs(*(uint16_t *) (curr_ptr + cnt)));
    cnt += sizeof(dest->q_class);

    return cnt;
}


int write_in_network_order(uint8_t* dest, struct dns_answer const* source) {
    int cnt = 0;

    vector<uint8_t> encoded_name = encode_dns_name(source->name);
    memcpy(dest + cnt, encoded_name.data(), encoded_name.size());
    cnt += encoded_name.size();

    *(uint16_t *) (dest + cnt) = htons(static_cast<uint16_t>(source->type));
    cnt += sizeof(source->type);

    *(uint16_t *) (dest + cnt) = htons(static_cast<uint16_t>(source->_class));
    cnt += sizeof(source->_class);

    *(uint32_t *) (dest + cnt) = htons(source->ttl);
    cnt += sizeof(source->ttl);

    *(uint16_t *) (dest + cnt) = htons(source->data_len);
    cnt += sizeof(source->data_len);

    for (int i = 0; i < source->data_len; i++) {
        (dest + cnt)[i] = source->rdata[i];
    }
    cnt += source->data_len;
//    switch (source->type) {
//        case DNS_TYPE_A: {
//            for (int i = 0; i < source->data_len; i++) {
//                (dest + cnt)[i] = source->rdata[i];
//            }
//            cnt += source->data_len;
//            break;
//        }
//        case DNS_TYPE_CNAME: {
//            memcpy(dest + cnt, source->rdata.c_str(), source->rdata.size());
//            cnt += source->rdata.size();
//
//            break;
//        }
//    }

    return cnt;
}

int extract_from_network_order(struct dns_answer* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff) {
    int cnt = 0;

    cnt += extract_encoded_name(
            &dest->name, curr_ptr + cnt, packet_buff
    );
    dest->name = decode_dns_name(dest->name);

    dest->type = static_cast<dns_record_type>(ntohs(*(uint16_t *) (curr_ptr + cnt)));
    cnt += sizeof(dest->type);

    dest->_class = static_cast<dns_record_class>(ntohs(*(uint16_t *) (curr_ptr + cnt)));
    cnt += sizeof(dest->_class);

    dest->ttl = ntohs(*(uint32_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->ttl);

    dest->data_len = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->data_len);

    for (int j = 0; j < dest->data_len; j++)
        dest->rdata.push_back((curr_ptr + cnt)[j]);
    cnt += dest->data_len;

    return cnt;
}


rdata_t convert_to_rdata(mx_rdata_t const* source) {
    uint16_t pref = htons(source->preference);
    vector<uint8_t> encoded_name = encode_dns_name(source->domain);

    vector<uint8_t> res(sizeof(pref) + encoded_name.size(), 0);

    *(uint16_t *) res.data() = pref;
    memcpy((uint8_t *) (res.data() + sizeof(pref)),
           encoded_name.data(), encoded_name.size());

    return res;
}

int extract_from_network_order(mx_rdata_t* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff) {
    int cnt = 0;

    dest->preference = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->preference);

    cnt += extract_encoded_name(&dest->domain, curr_ptr + cnt, packet_buff);
    dest->domain = decode_dns_name(dest->domain);

    return cnt;
}


rdata_t convert_to_rdata(soa_rdata_t const* source) {
    rdata_t res;

    auto tmp = encode_dns_name(source->primary_server);
    res.insert(res.end(), tmp.begin(), tmp.end());
    tmp = encode_dns_name(source->admin_mail_server);
    res.insert(res.end(), tmp.begin(), tmp.end());

    uint32_t serial_num = htonl(source->serial_num);
    uint32_t refresh_interval = htonl(source->refresh_interval);
    uint32_t retry_interval = htonl(source->retry_interval);
    uint32_t expire_limit = htonl(source->expire_limit);
    uint32_t max_ttl = htonl(source->max_ttl);

    tmp = vector<uint8_t>((uint8_t*) &serial_num, (uint8_t*) &serial_num + sizeof(serial_num));
    res.insert(res.end(), tmp.begin(), tmp.end());
    tmp = vector<uint8_t>((uint8_t*) &refresh_interval, (uint8_t*) &serial_num + sizeof(refresh_interval));
    res.insert(res.end(), tmp.begin(), tmp.end());
    tmp = vector<uint8_t>((uint8_t*) &retry_interval, (uint8_t*) &serial_num + sizeof(retry_interval));
    res.insert(res.end(), tmp.begin(), tmp.end());
    tmp = vector<uint8_t>((uint8_t*) &expire_limit, (uint8_t*) &serial_num + sizeof(expire_limit));
    res.insert(res.end(), tmp.begin(), tmp.end());
    tmp = vector<uint8_t>((uint8_t*) &max_ttl, (uint8_t*) &serial_num + sizeof(max_ttl));
    res.insert(res.end(), tmp.begin(), tmp.end());

    return res;
}

int extract_from_network_order(soa_rdata_t* dest, uint8_t const* curr_ptr, uint8_t const* packet_buff) {
    int cnt = 0;

    cnt += extract_encoded_name(&dest->primary_server, curr_ptr + cnt, packet_buff);
    dest->primary_server = decode_dns_name(dest->primary_server);

    cnt += extract_encoded_name(&dest->admin_mail_server, curr_ptr + cnt, packet_buff);
    dest->admin_mail_server = decode_dns_name(dest->admin_mail_server);

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





int write_in_network_order(uint8_t* dest, struct dns_message const* src) {
    uint8_t* curr = dest;

    // write header
    curr += write_in_network_order(curr, &src->header);

    // write queries
    size_t no_queries = src->header.query_count;
    if (no_queries != src->queries.size()) {cout << "non matching no_queries" << endl; throw;}
    for (int i = 0; i < no_queries; i++) {
        curr += write_in_network_order(curr, &src->queries[i]);
    }

    // write ans
    size_t no_ans = src->header.ans_count;
    if (no_ans != src->ans.size()) {cout << "non matching no_queries" << endl; throw;}
    for (int i = 0; i < no_ans; i++) {
        curr += write_in_network_order(curr, &src->ans[i]);
    }

    // write ans auth
    size_t no_ans_auth = src->header.auth_count;
    if (no_ans_auth != src->ans_auth.size()) {cout << "non matching no_queries" << endl; throw;}
    for (int i = 0; i < no_ans_auth; i++) {
        curr += write_in_network_order(curr, &src->ans_auth[i]);
    }

    // write and addit
    size_t no_ans_addit = src->header.add_count;
    if (no_ans_addit != src->ans_addit.size()) {cout << "non matching no_queries" << endl; throw;}
    for (int i = 0; i < no_ans_addit; i++) {
        curr += write_in_network_order(curr, &src->ans_addit[i]);
    }

    return curr - dest;
}

int extract_from_network_order(struct dns_message* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    // read header
    curr += extract_from_network_order(&dest->header, curr);

    // read queries
    dest->queries.resize(dest->header.query_count);
    for (int i = 0; i < dest->header.query_count; i++) {
        curr += extract_from_network_order(&dest->queries[i], curr, src);
    }

    // read ans
    dest->ans.resize(dest->header.ans_count);
    for (int i = 0; i < dest->header.ans_count; i++) {
        curr += extract_from_network_order(&dest->ans[i], curr, src);
    }

    // read ans auth
    dest->ans_auth.resize(dest->header.auth_count);
    for (int i = 0; i < dest->header.auth_count; i++) {
        curr += extract_from_network_order(&dest->ans_auth[i], curr, src);
    }

    // read and addit
    dest->ans_addit.resize(dest->header.add_count);
    for (int i = 0; i < dest->header.add_count; i++) {
        curr += extract_from_network_order(&dest->ans_addit[i], curr, src);
    }

    return curr - src;
}






void print_dns_message(struct dns_message const* msg, bool print_queries, bool print_ans) {
    uint16_t no_queries = msg->header.query_count;
    cout << "number of queries: " << no_queries << endl;

    uint16_t no_ans = msg->header.ans_count;
    cout << "number of answer: " << no_ans << endl;

    uint16_t no_authoritative_answers = msg->header.auth_count;
    cout << "number of authoritative answers: " << no_authoritative_answers << endl;

    uint16_t no_addit_data = msg->header.add_count;
    cout << "number of additional ans: " << no_addit_data << endl;

    // print queries
    if (no_queries > 0 && print_queries) {
        cout << "queries:" << endl;

        for (int i = 0; i < no_queries; i++) {
            cout << "name: " << (char*) msg->queries[i].q_name.data() << ", ";
            if (auto tmp = dns_record_type_to_str(msg->queries[i].q_type); tmp.has_value()) cout << "type: " << tmp.value();
            else cout << "type: unknown";

            cout << endl;
        }
    }

    // print answers
    if (no_ans > 0 && print_ans) {
        cout << "answers:" << endl;

        for (int i = 0; i < no_ans; i++) {
            cout << "name: " << (char*) msg->ans[i].name.data() << ", ";
            if (msg->ans[i].type == dns_record_type::a) {
                ip4_addr addr;
                extract_from_network_order(&addr, (uint8_t *) msg->ans[i].rdata.data());
                cout << "has ip4 addr: " << ip4_addr_to_str(addr);

            } else if (msg->ans[i].type == dns_record_type::cname) {
                // cout << "has alias key: " << (char*) decoded_name.data();

            } else if (msg->ans[i].type == dns_record_type::mx) {
                // struct mx_rdata_t rdata;
                // extract_from_network_order(&rdata, (uint8_t *) msg->ans[i].rdata.data(), buff);
                // cout << "has mail aggregator: " << (char*) rdata.domain.data();
            }

            cout << endl;
        }
    }

    // print authorities
    if (no_authoritative_answers > 0 && print_ans) {
        cout << "authoritative answers:" << endl;

        for (int i = 0; i < no_authoritative_answers; i++) {
            cout << "name: " << (char*) msg->ans_auth[i].name.data() << ", ";
            if (msg->ans_auth[i].type == dns_record_type::ns) {
                cout << "has nameserver: " << (char*) msg->ans_auth[i].rdata.data();

            } else if (msg->ans_auth[i].type == dns_record_type::soa) {
                // struct soa_rdata_t rdata;
                // extract_from_network_order(&rdata, (uint8_t *) msg->ans_auth[i].rdata.data(), buff);
                // cout << "start of authority:" << endl;
                // cout << "\t" << "primary server: " << (char*) rdata.primary_server.data() << endl;
                // cout << "\t" << "responsible mailbox: " << (char*) rdata.admin_mail_server.data() << endl;

            }

            cout << endl;
        }
    }

    // print additional
    if (no_addit_data > 0 && print_ans) {
        cout << "additional answers:" << endl;

        for (int i = 0; i < no_addit_data; i++) {
            cout << "name: " << (char*) msg->ans_addit[i].name.data() << ", ";
            if (msg->ans_addit[i].type == dns_record_type::a) {
                ip4_addr addr;
                extract_from_network_order(&addr, (uint8_t *) msg->ans_addit[i].rdata.data());
                cout << "has ip4 addr: " << ip4_addr_to_str(addr);

            }

            cout << endl;
        }
    }

}


void dns_protocol::send(shared_ptr<net_access> const& net_access, dns_message const& msg) {
    thread_local static uint8_t buff[0x10000];
    auto buff_sz = write_in_network_order(buff, &msg);

    net_access->get_send_access()->send_data(vector<uint8_t>(buff, buff + buff_sz));
}

void dns_protocol::connect_recv(
    shared_ptr<net_access> const& net_access, shared_ptr<recv_listener<struct dns_message>> const& recv
) {
    struct my_recv : public recv_listener_bytes {
        shared_ptr<recv_listener<struct dns_message>> recv;
        my_recv(shared_ptr<recv_listener<dns_message>> const& recv) : recv(recv) {}

        void handle_recv(vector<uint8_t> const& data) override {
            struct dns_message header;
            extract_from_network_order(&header, data.data());

            recv->handle_recv(header);
        }
    };

    net_access->set_recv_access(make_shared<my_recv>(recv));
}