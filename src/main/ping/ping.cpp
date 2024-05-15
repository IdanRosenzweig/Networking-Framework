#include "../../temp_utils/ping/ping_util.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void ping_main(ip4_addr dest_ip, int count) {

    ping_util pinger;
//    ping_util pinger(new interface_gateway("virt0"));

    pinger.dest_ip.set_next_choice(dest_ip);
    pinger.count.set_next_choice(count);
    pinger.delay_interval.set_next_choice(1000ms);

    pinger.ping_node();

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("ping devices on ip network");
    desc.add_options()
            ("help", "print tool use description")
            ("ip", po::value<string>(), "dest ip to ping")
            ("count", po::value<int>(), "number of times to ping. if no specified, 10 by default");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    if (!vm.count("ip")) {
        cout << desc << endl;
        return 1;
    }
    string dest = vm["ip"].as<string>();

    int count;
    if (!vm.count("count")) count = 10;
    else count = vm["count"].as<int>();

    ping_main(convert_to_ip4_addr(dest), count);

}