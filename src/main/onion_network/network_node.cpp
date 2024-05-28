#include "../../linux/osi/network_layer_gateway.h"
#include "../../temp_utils/onion_network/onion_network_node.h"
#include "../../linux/if/hardware.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void onion_network_node_main(const string& net_layer_iface) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(net_layer_iface);

    onion_network_node node(get_ip_addr_of_iface(net_layer_iface), new network_layer_gateway(iface_access));

    cout << "onion network node started" << endl;

    while (true) {
    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help,h", "print tool use description")
            ("net-layer-iface", po::value<string>(), "linux interface which can be used to send data to the network layer")
//            ("clients-iface", po::value<string>(), "linux interface to listen on to clients")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("net-layer-iface")) {
        std::cout << opts << endl;
        return 1;
    }
    string net_layer_iface = vm["net-layer-iface"].as<string>();

    onion_network_node_main(net_layer_iface);

}