#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#ifdef BUILD_OS_LINUX
#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/hardware.h"
#elifdef BUILD_OS_MACOS
#include "lib/macos/macos_iface.h"
#include "lib/macos/macos_iface_net_access.h"
#include "lib/macos/hardware.h"
#include "lib/macos/virtual_net.h"
#endif


#include "src/protocols/ether2/ethernet2_protocol.h"
#include "src/protocols/arp/arp_header.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/tools/arp_tool/arp_tool.h"

void arp_scan_single_main(string const& iface_name, ip4_addr ip_addr) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
#endif

    /* ether2 surface */
    shared_ptr<net_access> ether2_surface;
#ifdef BUILD_OS_LINUX
    ether2_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ether2_surface = iface_net_access;
#endif

    /* arp broadcast surface */
    shared_ptr<net_access> arp_broadcast_surface = make_shared<ethernet2_protocol::net_access_broadcast>(
        std::move(ether2_surface),
        get_mac_addr_of_iface(iface_name).value(),
        ethertype::arp
    );

    /* scanning */
    cout << "scanning for " << ip4_addr_to_str(ip_addr) << endl;
    mac_addr res = arp_tool::search_for_mac_addr(
        arp_broadcast_surface,
        ip_addr,
        get_mac_addr_of_iface(iface_name).value(), get_ip4_addr_of_iface(iface_name).value()
    );
    printf("associated mac addr: %s\n", mac_addr_to_str(res).c_str());
}

void arp_scan_subnet_main(string const& iface_name, ip4_subnet_mask subnet, chrono::duration<int64_t, milli> delay) {
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* network access in linux */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);

    /* ether2 surface */
    shared_ptr<net_access> ether2_surface = iface_net_access;

    /* arp broadcast surface */
    shared_ptr<net_access> arp_broadcast_surface = make_shared<ethernet2_protocol::net_access_broadcast>(
        std::move(ether2_surface),
        get_mac_addr_of_iface(iface_name).value(),
        ethertype::arp
    );

    /* scanning */
    cout << "scanning the subnet " << ip4_subnet_mask_to_str(subnet) << endl;
    auto entries = arp_tool::scan_entire_subnet(
        arp_broadcast_surface,
        subnet,
        {get_ip4_addr_of_iface(iface_name).value()},
        get_mac_addr_of_iface(iface_name).value(), get_ip4_addr_of_iface(iface_name).value()
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