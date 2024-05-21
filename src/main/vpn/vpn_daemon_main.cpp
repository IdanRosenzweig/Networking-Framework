#include "../../temp_utils/vpn/vpn_daemon.h"

#include <string>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;

void vpn_daemon_main(const string &local_iface) {
    vpn_daemon vpnDaemon(local_iface);

    while (true) {

    }
}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("local-iface", po::value<string>(), "linux interface to use to connect to the local network")
//            ("extern-iface", po::value<string>(), "linux interface to listen on to client")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("local-iface")) {
        cout << opts << endl;
        return 1;
    }
    string local_iface = vm["local-iface"].as<string>();

    vpn_daemon_main(local_iface);

}