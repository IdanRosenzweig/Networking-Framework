#include "../../temp_utils/dns_server_client/dns_server.h"
#include "../../temp_utils/dns_server_client/database/database_loader.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void dns_server_main(const string& db_path) {
    dns_server server;

    load_database(&server, db_path);

    while (true) {
    }

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "print tool use description")
            ("db", po::value<string>(),
             "path to database file ");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    if (!vm.count("db")) {
        cout << desc << endl;
        return 1;
    }
    string db_path = vm["db"].as<string>();

    dns_server_main(db_path);

}