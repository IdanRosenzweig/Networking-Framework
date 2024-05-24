#include "../../temp_utils/dns_server_client/dns_server.h"
#include "../../temp_utils/dns_server_client/database_loader.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/data_link_layer_gateway.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void dns_server_main(const string& iface, const string& db_path) {
    dns_server server(get_ip_addr_of_iface(iface), new network_layer_gateway(iface));

    load_database(&server, db_path);

    while (true) {
    }

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("db", po::value<string>(),
             "path to database file ");

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

    if (!vm.count("db")) {
        std::cout << opts << endl;
        return 1;
    }
    string db_path = vm["db"].as<string>();

    dns_server_main(iface, db_path);

}