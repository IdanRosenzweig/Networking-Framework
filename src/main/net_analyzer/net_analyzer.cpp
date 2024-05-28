#include "../../linux/if/wrappers/interface_sniffer.h"
#include "../../temp_utils/analyzer/net_analyzer.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void net_analyzer_main(const string& iface) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    interface_sniffer sniffer(iface_access);
    net_analyzer analyzer(&sniffer);

    while (true) {

    }
}

namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("Allowed options");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "interface to sniff traffic from")
//            ("hd", po::value<bool>(), "show hexdump of packets")
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

    net_analyzer_main(iface);

}