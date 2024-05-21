#include "../../temp_utils//ping/traceroute_util.h"
#include "../../linux/hardware.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void traceroute_main(const string& iface, ip4_addr dest_ip) {
    traceroute_util tracer(get_ip_addr_of_iface(iface), new network_layer_gateway(iface));
//    traceroute_util tracer(new interface_gateway("virt0"));

    tracer.dest_ip.set_next_choice(dest_ip);

    tracer.trace_to_destination();
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("search path on ip network to dest device");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("ip", po::value<string>(), "dest ip to search path to");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        cout << opts << endl;
        return 1;
    }
    string interface = vm["interface"].as<string>();

    if (!vm.count("ip")) {
        cout << opts << endl;
        return 1;
    }
    string dest = vm["ip"].as<string>();

    traceroute_main(interface, convert_to_ip4_addr(dest));

//    traceroute_main("enp0s3", convert_to_ip4_addr(""));
}

