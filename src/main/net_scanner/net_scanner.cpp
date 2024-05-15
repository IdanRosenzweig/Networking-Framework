#include "../../linux/interface_gateway.h"
#include "../../temp_utils/net_arp/net_arp.h"
#include "../../linux/hardware.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void arp_scan_single_main(ip4_addr ip_addr) {
    net_arp scanner;

    cout << "mac address associated with ip: " << convert_to_str(ip_addr) << endl;
    print_mac(scanner.search_for_mac_addr(ip_addr));
}

void arp_scan_subnet_main(ip4_subnet_mask mask) {
    net_arp scanner;

    auto results = scanner.scan_entire_subnet(mask);

    cout << endl << "search result for subnet: " << convert_to_str(mask) << endl;
    for (auto& pair : results) {
        cout << convert_to_str(pair.first) << " is ";
        print_mac(pair.second);
        cout << endl;
    }

}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "print tool use description")
            ("ip", po::value<string>(), "scan for single ip address")
            ("subnet", po::value<string>(), "scan all ip addresses in a subnet");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    if (vm.count("ip")) {
        arp_scan_single_main(convert_to_ip4_addr(vm["ip"].as<string>()));
        return 0;
    }
    if (vm.count("subnet")) {
        arp_scan_subnet_main(convert_to_ip4_subnet_mask(vm["subnet"].as<string>()));
        return 0;
    }

}