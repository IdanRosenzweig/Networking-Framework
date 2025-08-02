#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <vector>
#include <memory>
using namespace std;

#include "src/abstract/sniffer/sniffer.h"
#include "src/abstract/sniffer/basic_sniff_handler.h"

/* sniffed packet */
enum class net_analyzer_capture_dir {
    outgoing, // outoing data
    incoming, // incoming data
    // self_incoming // incoming data, came from local
};

struct net_analyzer_sniffed_packet {
    chrono::time_point<chrono::system_clock> time;
    net_analyzer_capture_dir capture_dir;
    vector<uint8_t> data;
};

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

/* net analyzer */
struct net_analyzer {
private:
    shared_ptr<sniffer_recv> sniffer_outgoing;
    shared_ptr<sniffer_recv> sniffer_incoming;

    template <net_analyzer_capture_dir dir>
    struct sniff_handler : public basic_sniff_handler<vector<uint8_t>> {
    private:
        net_analyzer* par;

    public:
        sniff_handler(net_analyzer* par) : par(par) {
        }

        void handle_sniff(vector<uint8_t> const& val) override {
            // create the sniffed packet struct
            shared_ptr<net_analyzer_sniffed_packet> sniffed_packet = make_shared<net_analyzer_sniffed_packet>(
                chrono::system_clock::now(),
                dir,
                val
            );

            // parse the packet
            net_analyzer_parsed_packet parsed_packet = parse_sniffed_packet(sniffed_packet);

            // print the packet
            print_parsed_packet(parsed_packet);

        }
    };
    
public:
    net_analyzer(shared_ptr<sniffer_recv> const& _sniffer_outgoing, shared_ptr<sniffer_recv> const& _sniffer_incoming)
    : sniffer_outgoing(_sniffer_outgoing), sniffer_incoming(_sniffer_incoming) {

        sniffer_outgoing->add_handler(make_shared<sniff_handler<net_analyzer_capture_dir::outgoing>>(this));
        sniffer_incoming->add_handler(make_shared<sniff_handler<net_analyzer_capture_dir::incoming>>(this));

    }


};