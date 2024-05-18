#include "../../temp_utils//ping/traceroute_util.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void traceroute_main(ip4_addr dest_ip) {
    traceroute_util tracer(new interface_gateway("virt0"));
    tracer.dest_ip.set_next_choice(dest_ip);

    tracer.trace_to_destination();

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("search path on ip network to dest device");
    desc.add_options()
            ("help", "print tool use description")
            ("ip", po::value<string>(), "dest ip to search path to");

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

    traceroute_main(convert_to_ip4_addr(dest));

}

