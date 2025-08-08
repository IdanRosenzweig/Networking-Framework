#include <boost/program_options.hpp>

#include <iostream>
#include <chrono>
#include <optional>
using namespace std;

#ifdef BUILD_OS_LINUX
#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/virtual_net.h"
#include "lib/linux/hardware.h"
#elifdef BUILD_OS_MACOS
#include "lib/macos/macos_iface.h"
#include "lib/macos/macos_iface_net_access.h"
#include "lib/macos/hardware.h"
#include "lib/macos/virtual_net.h"
#endif

#include "src/protocols/ether2/ethernet2_protocol.h"

#include "src/protocols/ip4/ip4_protocol.h"
#include "src/protocols/ip4/ip4_routing_table.h"

#include "src/tools/ping/ping_tool.h"


void ping_main(string const& iface_name, ip4_addr dest_ip, optional<size_t> cnt, std::chrono::milliseconds interval) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
#endif

    /* ether2 surface */
    shared_ptr<net_access> ether2_surface;
#ifdef BUILD_OS_LINUX
    ether2_surface = iface_net_access;
#elifdef BUILD_OS_MACOS
    ether2_surface = iface_net_access;
#endif
    multi_net_access ether2_multi_surface(std::move(ether2_surface));

    // my routing table for the ip4 network 
    ip4_routing_table routing_table;
    routing_table.set_default_gateway(
        make_shared<ethernet2_protocol::net_access_single>(
            ether2_multi_surface.generate_net_access(),
            str_to_mac_addr("c4:eb:42:ed:c5:b7").value(),
            get_mac_addr_of_iface(iface_name).value(),
            ethertype::ip4
        )
    );
    auto routed_ip4_send_surface = ip4_protocol::create_send_access_from_routing_table(std::move(routing_table));

    shared_ptr<net_access> ip4_recv_surface = make_shared<ethernet2_protocol::net_access_broadcast>(
        ether2_multi_surface.generate_net_access(),
        get_mac_addr_of_iface(iface_name).value(),
        ethertype::ip4
    );

    shared_ptr<net_access> ip4_surface = net_access_save_send_access(std::move(ip4_recv_surface), std::move(routed_ip4_send_surface));

    auto icmp_surface = make_shared<ip4_protocol::net_access_single>(
        std::move(ip4_surface),
        dest_ip,
        get_ip4_addr_of_iface(iface_name).value(),
        ip_prot_values::icmp
    );

    ping_tool::ping(icmp_surface, cnt, interval);

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("ping device in ip network");
    opts.add_options()
        ("help,h", "print tool use description")

        ("iface", po::value<string>(), "linux interface to use")

        ("dest", po::value<string>(), "dest ip to ping")

        ("interval,i", po::value<size_t>(), "interval in ms between pings. 1000ms by default")
        ("count,c", po::value<size_t>(), "number of times to ping. if not specified, continues forever")
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
    string interface = vm["iface"].as<string>();

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

    optional<size_t> count = {};
    if (vm.count("count")) count = vm["count"].as<size_t>();

    chrono::milliseconds interval;
    if (!vm.count("interval")) interval = chrono::milliseconds(1000);
    else interval = chrono::milliseconds(vm["interval"].as<int>());

    ping_main(interface, dest, count, std::chrono::milliseconds(interval));

}