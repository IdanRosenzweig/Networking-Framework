#include "../../tools/vpn/vpn_daemon.h"

#include "../../linux/if/iface_access_point.h"

#include <string>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;

void vpn_daemon_main(const string &local_iface, const string &extern_iface) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(local_iface);

    vpn_daemon vpnDaemon(iface_access);

    cout << "daemon started" << endl;

    while (true) {

    }
}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("vpn daemon");
    opts.add_options()
            ("help,h", "print tool use description")
            ("local-iface", po::value<string>(), "linux interface to use to connect to the local network")
            ("extern-iface", po::value<string>(), "linux interface to listen on to the vpn client. if not specified, uses the same interface as local-iface")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("local-iface")) {
        std::cout << opts << endl;
        return 1;
    }
    string local_iface = vm["local-iface"].as<string>();

    string extern_iface;
    if (!vm.count("extern-iface")) extern_iface = local_iface;
    else extern_iface = vm["extern-iface"].as<string>();

    vpn_daemon_main(local_iface, extern_iface);

}