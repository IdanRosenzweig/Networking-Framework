#include "../../linux/interface_gateway.h"
#include "../../temp_utils/net_arp/net_arp.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void arp_scan_single_main(const string& iface, ip4_addr ip_addr) {
    net_arp scanner(new data_link_layer_gateway(iface));

    std::cout << "mac address associated with ip: " << convert_to_str(ip_addr) << endl;
    print_mac(scanner.search_for_mac_addr(ip_addr, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface)));
}

void arp_scan_subnet_main(const string& iface, ip4_subnet_mask mask) {
    net_arp scanner(new data_link_layer_gateway(iface));

    auto results = scanner.scan_entire_subnet(mask, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface));

    std::cout << endl << "search result for subnet: " << convert_to_str(mask) << endl;
    for (auto& pair : results) {
        std::cout << convert_to_str(pair.first) << " is ";
        print_mac(pair.second);
        std::cout << endl;
    }

}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("ip", po::value<string>(), "scan for single ip address")
            ("subnet", po::value<string>(), "scan all ip addresses in a subnet");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        std::cout << opts << endl;
        return 1;
    }
    string iface = vm["interface"].as<string>();

    if (vm.count("ip")) {
        arp_scan_single_main(iface, convert_to_ip4_addr(vm["ip"].as<string>()));
        return 0;
    }
    if (vm.count("subnet")) {
        arp_scan_subnet_main(iface, convert_to_ip4_subnet_mask(vm["subnet"].as<string>()));
        return 0;
    }

}