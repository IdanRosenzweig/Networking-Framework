#pragma once

#include <string>
#include <optional>
#include <vector>
#include <memory>
using namespace std;

#include "src/abstract/receiving/basic_recv_listener.h"

#include "sniffed_packet.h"

#include "src/protocols/ether2/ethernet2_header.h"
#include "src/protocols/arp/arp_header.h"
#include "src/protocols/ip4/ip4_header.h"


/* parsed packet */
enum class net_analyzer_prot {
    ethernet2,
    arp,
    ip4,
    udp,
    tcp,
    icmp,
    dns,
    mdns
};
string net_analyzer_prot_to_str(net_analyzer_prot prot);
optional<net_analyzer_prot> str_to_net_analyzer_prot(string const& str);

enum class net_analyzer_parsed_status {
    valid,
    corrupted,
    untested,
    unknown
};

struct net_analyzer_parsed_prot {
    net_analyzer_parsed_status status;
    optional<net_analyzer_prot> prot;
    string desc;
};

struct net_analyzer_parsed_packet {
    shared_ptr<net_analyzer_sniffed_packet> sniffed_packet;
    vector<unique_ptr<net_analyzer_parsed_prot>> prots;
    string desc;
};

net_analyzer_parsed_packet parse_sniffed_packet(shared_ptr<net_analyzer_sniffed_packet> const& sniffed_packet);

void print_parsed_packet(net_analyzer_parsed_packet const& packet);









struct my_recv_ether2;
struct my_recv_arp;
struct my_recv_ip4;

struct local_ctx {
    vector<unique_ptr<net_analyzer_parsed_prot>> res;
    shared_ptr<my_recv_ether2> recv_ether2;
    shared_ptr<my_recv_arp> recv_arp;
    shared_ptr<my_recv_ip4> recv_ip4;
};

struct my_recv_ether2 : public basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>> {
    local_ctx* ctx;
    my_recv_ether2(local_ctx* ctx) : ctx(ctx) {}

    void handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data);
};

struct my_recv_arp : public basic_recv_listener<arp_header> {
    local_ctx* ctx;
    my_recv_arp(local_ctx* ctx) : ctx(ctx) {}

    void handle_recv(arp_header const& data);
};

struct my_recv_ip4 : public basic_recv_listener<pair<ip4_header, vector<uint8_t>>> {
    local_ctx* ctx;
    my_recv_ip4(local_ctx* ctx) : ctx(ctx) {}

    void handle_recv(pair<ip4_header, vector<uint8_t>> const& data);
};

