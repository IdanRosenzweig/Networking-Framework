#include "../../linux/interface_gateway.h"
#include "../../temp_utils/bandwidth/bandwidth.h"

#include <unistd.h>
#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

void bandwidth_main(const string& iface) {
    interface_sniffer sniffer(iface);
    bandwidth indicator(&sniffer);

    while (true) {
        sleep(1);

        std::cout << "in: " << indicator.bytes_in_cnt << " B\t" << "out: " << indicator.bytes_out_cnt << " B" << endl;
        indicator.bytes_in_cnt = 0;
        indicator.bytes_out_cnt = 0;
    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
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

    bandwidth_main(iface);

}