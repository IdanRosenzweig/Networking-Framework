#include "../../temp_utils/dns_server_client/dns_client.h"
#include "../../linux/hardware.h"
#include "../../linux/osi/network_layer_gateway.h"

#include <unistd.h>
#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

void dns_client_main(const string& iface, dns_record_type type, const string &key, ip4_addr dns_server) {
    dns_client client(dns_server, get_ip_addr_of_iface(iface), new network_layer_gateway(iface));
//    dns_client client(dns_server, new interface_gateway("virt0"));

    client.query(type, key);

}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("server", po::value<string>(),
             "ip of dns server to use. uses 8.8.8.8 by default")
            ("type", po::value<string>(),
             "type of dns record (A, MX). uses A by default")
            ("key", po::value<string>(),
             "value of record key to search");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        cout << opts << endl;
        return 1;
    }
    string iface = vm["interface"].as<string>();

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
        cout << opts << endl;
        return 1;
    }
    string key = vm["key"].as<string>();

    dns_client_main(iface, str_to_record_type(type), key, convert_to_ip4_addr(server_ip));

//    dns_client_main("enp0s3", str_to_record_type("A"), "google.com", convert_to_ip4_addr("8.8.8.8"));

}