#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <vector>
#include <memory>
using namespace std;

#include "src/abstract/sniffer/sniffer.h"
#include "src/abstract/sniffer/basic_sniff_handler.h"

#include "sniffed_packet.h"
#include "parsed_packet.h"

/* net analyzer */
struct net_analyzer {
private:
    shared_ptr<sniffer_recv> sniffer;

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
                val
            );

            // parse the packet
            net_analyzer_parsed_packet parsed_packet = parse_sniffed_packet(sniffed_packet);

            // print the packet
            print_parsed_packet(parsed_packet);

        }
    };
    
public:
    net_analyzer(shared_ptr<sniffer_recv> const& _sniffer)
    : sniffer(_sniffer) {

        sniffer->add_handler(make_shared<sniff_handler>(this));

    }


};