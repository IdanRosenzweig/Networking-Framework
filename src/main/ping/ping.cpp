#include "../../tools/ping/ping_util.h"

#include "../../linux/if/iface_access_point.h"
#include "../../linux/if/hardware.h"
#include "../../linux/osi/network_layer_gateway.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void ping_main(const string& iface, ip4_addr dest_ip, int count, std::chrono::milliseconds interval) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    ping_util pinger(get_ip_addr_of_iface(iface), new network_layer_gateway(iface_access));

    pinger.dest_ip.set_next_choice(dest_ip);
    pinger.count.set_next_choice(count);
    pinger.delay_interval.set_next_choice(interval);

    pinger.ping_node();

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("ping devices on ip network");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("ip", po::value<string>(), "dest ip to ping")
            ("interval,i", po::value<int>(), "interval in ms between pings. 1000ms by default")
    ("count,c", po::value<int>(), "number of times to ping. 10 by default");

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

    if (!vm.count("ip")) {
        std::cout << opts << endl;
        return 1;
    }
    string dest = vm["ip"].as<string>();

    int count;
    if (!vm.count("count")) count = 10;
    else count = vm["count"].as<int>();

    int interval;
    if (!vm.count("interval")) interval = 1000;
    else interval = vm["interval"].as<int>();

    ping_main(interface, str_to_ip4_addr(dest), count, std::chrono::milliseconds(interval));

}