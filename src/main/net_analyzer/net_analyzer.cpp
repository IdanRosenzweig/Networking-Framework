#include "../../linux/interface_gateway.h"
#include "../../temp_utils/analyzer/net_analyzer.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void net_analyzer_main(const string& iface) {
    net_analyzer analyzer(iface);

    while (true) {

    }
}

namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "interface to sniff traffic from")
//            ("show-hexdump,h", po::value<bool>(), "show hexdump of packets")
            ;

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
    string iface = vm["interface"].as<string>();

    net_analyzer_main(iface);

}