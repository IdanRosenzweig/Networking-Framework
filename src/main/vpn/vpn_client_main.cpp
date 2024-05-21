#include "../../linux/interface_gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"

#include "../../temp_utils/vpn/vpn_client.h"
#include "../../linux/virtual_if.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void vpn_client_main(ip4_addr daemon_ip, ip4_addr new_ip, ip4_subnet_mask subnet) {
    vpn_client vpn(daemon_ip);

    char dev[6] = "virt0";
    linux_virtual_iface iface(&vpn, dev);

    cout << "virtual interface \"" << dev << "\" is open. send your data link layer traffic through it" << endl;

//    string set_ip_comm = "sudo ip addr add " + convert_to_str(new_ip) + "/32 dev virt0";
//    system(set_ip_comm.c_str());
//    system("sudo ip link set dev virt0 up");
//    string route_comm = "sudo ip route add " + convert_to_str(subnet) + " via " + convert_to_str(new_ip) + " dev virt0";
//    cout << route_comm << endl;
//    system(route_comm.c_str());

    while (true) {

    }
}

int main(int argc, char** argv) {
//    namespace po = boost::program_options;
//
//    po::options_description opts("Allowed options");
//    opts.add_options()
//            ("help", "print tool use description")
////            ("interface,i", po::value<string>(), "linux interface to use to connect to the daemon")
//            ("daemon,d", po::value<string>(), "ip of the daemon")
//            ("new-ip", po::value<string>(), "new ip to use when connecting to the remote network")
//            ("subnet", po::value<string>(), "ip subnet of the remote network")
//            ;
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, opts), vm);
//    po::notify(vm);
//
//    if (vm.count("help")) {
//        cout << opts << endl;
//        return 1;
//    }
//
//    if (!vm.count("daemon")) {
//        cout << opts << endl;
//        return 1;
//    }
//    string daemon_ip = vm["daemon"].as<string>();
//
//    if (!vm.count("new-ip")) {
//        cout << opts << endl;
//        return 1;
//    }
//    string new_ip = vm["new-ip"].as<string>();
//
//    if (!vm.count("subnet")) {
//        cout << opts << endl;
//        return 1;
//    }
//    string subnet = vm["subnet"].as<string>();
//
//    vpn_client_main(convert_to_ip4_addr(daemon_ip), convert_to_ip4_addr(new_ip), convert_to_ip4_subnet_mask(subnet));

    vpn_client_main(convert_to_ip4_addr("10.100.102.31"), convert_to_ip4_addr("10.100.102.44"),
                    convert_to_ip4_subnet_mask("10.100.102.0/24"));
}
