#include "../../temp_utils/ping/ping_util.h"
#include "../../linux/hardware.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void ping_main(const string& iface, ip4_addr dest_ip, int count) {

//    ping_util pinger(convert_to_ip4_addr("10.100.102.31"), new network_layer_gateway(iface));
    ping_util pinger(get_ip_addr_of_iface(iface), new network_layer_gateway(iface));

    pinger.dest_ip.set_next_choice(dest_ip);
    pinger.count.set_next_choice(count);
    pinger.delay_interval.set_next_choice(1000ms);

    pinger.ping_node();

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("ping devices on ip network");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("ip", po::value<string>(), "dest ip to ping")
            ("count", po::value<int>(), "number of times to ping. if no specified, 10 by default");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        std::cout << opts << endl;
        return 1;
    }
    string interface = vm["interface"].as<string>();

    if (!vm.count("ip")) {
        std::cout << opts << endl;
        return 1;
    }
    string dest = vm["ip"].as<string>();

    int count;
    if (!vm.count("count")) count = 10;
    else count = vm["count"].as<int>();

    ping_main(interface, convert_to_ip4_addr(dest), count);

//    ping_main("enp0s3", convert_to_ip4_addr("10.100.102.3"), 10);
}