#include "../../temp_utils/dns_server_client/dns_client.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void dns_client_main(dns_record_type type, const string &key, ip4_addr dns_server) {
    dns_client client(dns_server);
//    dns_client client(dns_server, new interface_gateway("virt0"));

    client.query(type, key);

}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "print tool use description")
            ("server", po::value<string>(),
             "ip of dns server to use. if no specified, uses 8.8.8.8")
            ("type", po::value<string>(),
             "type of dns record (A, MX). if not specified, chooses A by default")
            ("key", po::value<string>(),
             "value of record key to search");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    string server_ip;
    if (!vm.count("server")) {
        server_ip = "8.8.8.8";
    } else server_ip = vm["server"].as<string>();

    string type;
    if (!vm.count("type")) {
        type = "A";
    } else {
        type = vm["type"].as<string>();
        std::transform(type.begin(), type.end(), type.begin(), ::toupper);
    }

    if (!vm.count("key")) {
        cout << desc << endl;
        return 1;
    }
    string key = vm["key"].as<string>();

    dns_client_main(str_to_record_type(type), key, convert_to_ip4_addr(server_ip));
//    dns_client_main(DNS_TYPE_A, "google.com", convert_to_ip4_addr("8.8.8.8"));

    return 0;

}