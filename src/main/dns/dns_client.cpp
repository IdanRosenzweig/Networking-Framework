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

#include "src/protocols/udp/udp_protocol.h"

#include "src/tools/dns/dns_tool.h"

void dns_client_main(const string& iface_name, string const& key, dns_record_type type, dns_record_class _class, ip4_addr dns_server) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* virtual net access in linux */
    // shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    // vnet_net_access->connect(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
    /* virtual net access in linux */
    // shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    // vnet_net_access->connect(iface_name);
#endif

    /* ether2 surface */
    shared_ptr<net_access> ether2_surface; // can send and receive ether2 frames through this net_access
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

    shared_ptr<net_access> ip4_surface = net_access_save_send_access(std::move(ip4_recv_surface), std::move(routed_ip4_send_surface)); // can send and receive ip packets through this net_access

    shared_ptr<net_access> udp_surface = make_shared<ip4_protocol::net_access_single>( // can send and receive udp packets through this net_access
        std::move(ip4_surface),
        dns_server,
        get_ip4_addr_of_iface(iface_name).value(),
        ip_prot_values::udp
    );

    // can send and receive dns messages through this net_access
    shared_ptr<net_access> dns_surface = make_shared<udp_protocol::net_access_single>(
        std::move(udp_surface),
        DNS_SERVER_PORT,
        1234
    );

    // send dns requests
    dns_tool::send_requests(dns_surface, make_dns_query(key, type, _class));

}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("perform dns queries");
    opts.add_options()
            ("help,h", "print tool use description")

            ("iface", po::value<string>(), "linux interface to use")

            ("server", po::value<string>(), "ip of dns server to use. uses 8.8.8.8 by default")

            ("key", po::value<string>(), "value of record key to search")
            ("type", po::value<string>(), "type of dns record (a, mx, ptr). uses type (a) by default")
            ("class", po::value<string>(), "class of dns record (in, ch). uses class (in) by default")
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

    ip4_addr server_ip;
    if (!vm.count("server")) {
        std::cout << opts << endl;
        return 1;
    } else {
        if (auto tmp = str_to_ip4_addr(vm["server"].as<string>()); tmp.has_value()) server_ip = tmp.value();
        else {
            cout << "invalid dest ip" << endl;
            return 1;
        }
    }

    string key;
    if (!vm.count("key")) {
        std::cout << opts << endl;
        return 1;
    } else key = vm["key"].as<string>();

    dns_record_type type;
    if (!vm.count("type")) {
        type = dns_record_type::a;
    } else {
        if (auto tmp = str_to_dns_record_type(vm["type"].as<string>()); tmp.has_value()) type = tmp.value();
        else {
            cout << "invalid type" << endl;
            return 1;
        }
    }

    dns_record_class _class;
    if (!vm.count("class")) {
        _class = dns_record_class::in;
    } else {
        if (auto tmp = str_to_dns_record_class(vm["class"].as<string>()); tmp.has_value()) _class = tmp.value();
        else {
            cout << "invalid class" << endl;
            return 1;
        }
    }

    dns_client_main(iface, key, type, _class, server_ip);

}