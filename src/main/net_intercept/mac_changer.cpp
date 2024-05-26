#include "../../linux/interface_gateway.h"
#include "../../temp_utils/net_arp/net_arp.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <sys/ioctl.h>
using namespace std;

void change_local_mac_main(const string &iface, const string &assoc_iface, mac_addr new_mac) {
    cout << "current mac for " << assoc_iface << ": " << convert_to_str(get_mac_addr_of_iface(assoc_iface)) << endl;

    cout << "change the mac address inside the kernel and announcing it in broadcast" << endl << endl;
    set_mac_addr_for_iface(assoc_iface, new_mac);
    net_arp arp_util(new data_link_layer_gateway(iface));
    arp_util.announce_new_mac(get_ip_addr_of_iface(assoc_iface), new_mac);

    cout << "new mac for " << assoc_iface << ": " << convert_to_str(get_mac_addr_of_iface(assoc_iface)) << endl;

}


namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("change mac addresses");
    opts.add_options()
            ("help", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use to send frames")
            ("assoc-iface", po::value<string>(), "the associated interface to change its mac address")
            ("new-mac,m", po::value<string>(),
             "new mac address for the associated interface");

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

    if (!vm.count("assoc-iface")) {
        std::cout << opts << endl;
        return 1;
    }
    string assoc_iface = vm["assoc-iface"].as<string>();

    if (!vm.count("new-mac")) {
        std::cout << opts << endl;
        return 1;
    }
    string new_mac = vm["new-mac"].as<string>();

    change_local_mac_main(interface, assoc_iface, convert_to_mac_addr(new_mac));

}