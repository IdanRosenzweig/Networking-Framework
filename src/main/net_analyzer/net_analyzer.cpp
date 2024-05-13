#include "../../linux/data_link_layer/data_link_layer_gateway.h"
#include "../../temp_utils/analyzer/net_analyzer.h"

#include <unistd.h>
#include <boost/program_options.hpp>

void net_analyzer_main(const string& iface) {
    net_analyzer analyzer(iface);

    while (true) {

    }
}

namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "print tool use description")
            ("interface", po::value<string>(), "interface to sniff traffic from");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        cout << desc << endl;
        return 1;
    }
    string iface = vm["interface"].as<string>();

    net_analyzer_main(iface);

}