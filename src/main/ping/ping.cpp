#include <boost/program_options.hpp>

#include <iostream>
#include <chrono>
#include <optional>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/hardware.h"

#include "src/protocols/ether2/ethernet2_protocol.h"

#include "src/protocols/ip4/ip4_protocol.h"
#include "src/protocols/ip4/ip4_routing_table.h"

#include "src/tools/ping/ping_tool.h"


void ping_main(string const& iface_name, ip4_addr dest_ip, optional<size_t> cnt, std::chrono::milliseconds interval) {
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    iface_net_access->get_send_access()->send_data(vector<uint8_t>(100, 0x1));

    // my routing table for the ip4 network 
    ip4_routing_table routing_table;
    routing_table.set_default_gateway(
        make_shared<ethernet2_protocol::net_access_single>(
            std::move(iface_net_access),
            str_to_mac_addr("c4:eb:42:ed:c5:b7").value(),
            get_mac_addr_of_iface(iface_name).value(),
            ethertype::ip4
        )
    );

    auto routed_net_access = ip4_protocol::create_net_access_from_routing_table(std::move(routing_table));

    routed_net_access->get_send_access()->send_data(vector<uint8_t>(100, 0x2));

    auto server_net_access = make_shared<ip4_protocol::net_access_single>(
        std::move(routed_net_access),
        dest_ip,
        get_ip4_addr_of_iface(iface_name).value(),
        ip_prot_values::icmp
    );

    server_net_access->get_send_access()->send_data(vector<uint8_t>(100, 0x3));


    ping_tool::ping(server_net_access, cnt, interval);

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("ping devices on ip network");
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

    cout << "starting5" << endl;
    ping_main(interface, dest, count, std::chrono::milliseconds(interval));

}