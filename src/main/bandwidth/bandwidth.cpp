#include "../../linux/if/wrappers/interface_gateway.h"
#include "../../linux/if/wrappers/interface_sniffer.h"

#include "../../temp_utils/bandwidth/bandwidth.h"

#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

void bandwidth_main(const string& iface, std::chrono::milliseconds interval) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    interface_sniffer sniffer(iface_access);
    bandwidth indicator(&sniffer);

    while (true) {
        std::this_thread::sleep_for(interval);

        uint64_t out = indicator.bytes_out_cnt;
        uint64_t in = indicator.bytes_in_cnt;
        printf("bytes out: %-10lu   bytes in: %-10lu\n", out, in);
        indicator.bytes_in_cnt = 0;
        indicator.bytes_out_cnt = 0;
    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("interval,i", po::value<int>(), "interval (in ms) between status checks. 1000ms by default")
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

    int interval;
    if (!vm.count("interval"))
        interval = 1000;
    else interval = vm["interval"].as<int>();

    bandwidth_main(iface, std::chrono::milliseconds(interval));

}