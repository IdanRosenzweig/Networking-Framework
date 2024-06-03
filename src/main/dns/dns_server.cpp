#include "../../tools/dns/dns_server.h"

#include "../../linux/if/hardware.h"
#include "../../linux/if/iface_access_point.h"
#include "../../linux/osi/network_layer_gateway.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void dns_server_main(const string& iface, const string& db_path) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    dns_server server(get_ip_addr_of_iface(iface), new network_layer_gateway(iface_access));
    load_database(&server, db_path);

    while (true) {
    }

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("start dns sever daemon");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")
            ("db", po::value<string>(),
             "path to database file ");

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

    if (!vm.count("db")) {
        std::cout << opts << endl;
        return 1;
    }
    string db_path = vm["db"].as<string>();

    dns_server_main(iface, db_path);

}