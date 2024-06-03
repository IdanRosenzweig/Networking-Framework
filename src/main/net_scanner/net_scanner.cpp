#include "../../tools/net_arp/net_arp.h"

#include "../../linux/if/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void arp_scan_single_main(const string& iface, ip4_addr ip_addr) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    net_arp scanner(new data_link_layer_gateway(iface_access));

    std::cout << "mac address associated with ip: " << convert_to_str(ip_addr) << endl;
    cout << convert_to_str(scanner.search_for_mac_addr(ip_addr, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface))) << endl;
}

void arp_scan_subnet_main(const string& iface, ip4_subnet_mask mask, chrono::duration<int64_t, milli> delay) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    net_arp scanner(new data_link_layer_gateway(iface_access));

    auto results = scanner.scan_entire_subnet(mask, get_mac_addr_of_iface(iface), get_ip_addr_of_iface(iface), delay);

    std::cout << endl << "search result for subnet: " << convert_to_str(mask) << endl;
    for (auto& pair : results) {
        std::cout << convert_to_str(pair.first) << " is " << convert_to_str(pair.second) << endl;
    }

}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("scan local network for devices");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("ip", po::value<string>(), "scan for single ip address")
            ("subnet", po::value<string>(), "scan all ip addresses in a subnet")
            ("delay,d", po::value<int>(), "if scanning a subnet, this specifies the delay between the scanning of individual address (in ms). default is 5ms")
            ;

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

    if (vm.count("ip")) {
        arp_scan_single_main(iface, convert_to_ip4_addr(vm["ip"].as<string>()));
        return 0;
    }
    if (vm.count("subnet")) {
        int delay;
        if (!vm.count("delay")) delay = 5;
        else delay = vm["ip"].as<int>();

        arp_scan_subnet_main(iface, convert_to_ip4_subnet_mask(vm["subnet"].as<string>()),std::chrono::milliseconds(delay));
        return 0;
    }

}