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

void arp_scan_single_main(string const& iface_name, ip4_addr ip_addr) {
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* net access for ethernet broadcast arp */
    shared_ptr<net_access_bytes> net_access_eth_broadcast_arp = make_shared<ethernet2_protocol::net_access_broadcast>(iface_net_access, get_mac_addr_of_iface("enp0s3").value(), ethertype::arp);

    /* scanning */
    cout << "scanning for " << ip4_addr_to_str(ip_addr) << endl;
    mac_addr res = arp_tool::search_for_mac_addr(
        net_access_eth_broadcast_arp,
        ip_addr,
        get_mac_addr_of_iface(iface_name).value(), get_ip_addr_of_iface(iface_name).value()
    );
    printf("associated mac addr: %s\n", mac_addr_to_str(res).c_str());
}

void arp_scan_subnet_main(string const& iface_name, ip4_subnet_mask subnet, chrono::duration<int64_t, milli> delay) {
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* net access for ethernet broadcast arp */
    shared_ptr<net_access_bytes> net_access_eth_broadcast_arp = make_shared<ethernet2_protocol::net_access_broadcast>(iface_net_access, get_mac_addr_of_iface("enp0s3").value(), ethertype::arp);

    /* scanning */
    cout << "scanning the subnet " << ip4_subnet_mask_to_str(subnet) << endl;
    auto entries = arp_tool::scan_entire_subnet(
        net_access_eth_broadcast_arp,
        subnet,
        get_mac_addr_of_iface(iface_name).value(), get_ip_addr_of_iface(iface_name).value()
    );
    for (auto const& entry : entries) {
        printf("%s is %s\n", ip4_addr_to_str(entry.first).c_str(), mac_addr_to_str(entry.second).c_str());
    }

}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("scan local using arp");
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
        ip4_addr addr;
        if (auto tmp = str_to_ip4_addr(vm["ip"].as<string>()); tmp.has_value()) addr = tmp.value();
        else {
            cout << "invalid ip" << endl;
            return 1;
        }

        arp_scan_single_main(iface, addr);

    } else if (vm.count("subnet")) {
        int delay;
        if (!vm.count("delay")) delay = 5;
        else delay = vm["ip"].as<int>();

        ip4_subnet_mask subnet;
        if (auto tmp = str_to_ip4_subnet_mask(vm["subnet"].as<string>()); tmp.has_value()) subnet = tmp.value();
        else {
            cout << "invalid subnet" << endl;
            return 1;
        }

        arp_scan_subnet_main(iface, subnet, std::chrono::milliseconds(delay));

    } else {
        std::cout << opts << endl;
        return 1;
    }

}