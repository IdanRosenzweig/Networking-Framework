#include "../../tools/vpn/vpn_client.h"

#include "../../linux/if/iface_access_point.h"
#include "../../linux/if/wrappers/interface_gateway.h"
#include "../../linux/if/virtual/virtual_if.h"
#include "../../linux/if/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include "../../tools/net_arp/net_arp.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void vpn_client_main(const string& iface, const string& _new_iface_name, ip4_addr daemon_ip, ip4_addr new_ip, ip4_subnet_mask subnet) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

//    vpn_client vpn(daemon_ip, new interface_gateway(iface_access));
    vpn_client vpn(daemon_ip, iface);

    // virtual interface
    string virt_iface_name = _new_iface_name;
    linux_virtual_iface virt_iface(&vpn, virt_iface_name);

    cout << "virtual interface \"" << virt_iface_name << "\" was created. send your buff link layer traffic through it" << endl;

    // routing
    string assign_ip_comm = "sudo ip addr add " + convert_to_str(new_ip) + "/32 dev " + virt_iface_name;
    system(assign_ip_comm.c_str());
    string set_up_comm = "sudo ip link set dev " + virt_iface_name + " up";
    system(set_up_comm.c_str());
    string route_comm = "sudo ip route add " + convert_to_str(subnet) + " via " + convert_to_str(new_ip) + " dev " + virt_iface_name;
    system(route_comm.c_str());

    cout << "routing was added" << endl;


    while (true) {

    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("opens a virtual interface that is logically connected to a remote server's local network. send any data link layer traffic through it, and receive back ");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use to connect to the daemon")
            ("daemon-ip,d", po::value<string>(), "ip of the daemon")
            ("new-ip", po::value<string>(), "new ip to use when connecting to the remote network")
            ("subnet", po::value<string>(), "ip subnet of the remote network")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("iface")) {
        cout << opts << endl;
        return 1;
    }
    string iface = vm["iface"].as<string>();

    if (!vm.count("daemon-ip")) {
        cout << opts << endl;
        return 1;
    }
    string daemon_ip = vm["daemon-ip"].as<string>();

    if (!vm.count("new-ip")) {
        cout << opts << endl;
        return 1;
    }
    string new_ip = vm["new-ip"].as<string>();

    if (!vm.count("subnet")) {
        cout << opts << endl;
        return 1;
    }
    string subnet = vm["subnet"].as<string>();

    vpn_client_main(iface, "virt0", str_to_ip4_addr(daemon_ip), str_to_ip4_addr(new_ip), convert_to_ip4_subnet_mask(subnet));

}
