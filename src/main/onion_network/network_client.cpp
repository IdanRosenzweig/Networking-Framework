#include <iostream>

#include "../../temp_utils/proxy/ip_proxy_client.h"

#include "../../temp_connections/udp_client_server/udp_client.h"
#include "../../temp_utils/dns_server_client/dns_client.h"
#include "../../temp_connections/icmp/icmp_connection_client.h"
#include "../../temp_utils/onion_network/onion_network_client.h"

#include "../../linux/virtual_if.h"
#include <unistd.h>
#include <boost/program_options.hpp>

#define MY_IP "10.100.102.18"


void onion_network_node_main(const vector<ip4_addr>& path) {
    onion_network_client proxy(path);

    char dev[6] = "virt0";
    linux_virtual_iface iface(&proxy, dev);

    cout << "virtual interface \"" << dev << "\" is open. send your ip network through it" << endl;
    while (true) {

    }

//     regular dns communication
    {
//        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), &proxy);
        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), new interface_gateway("virt0"));
//        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"));

        // communicate
        vector<string> hosts = {
            "wiki.osdev.org",
            "www.wix.com",
            "docs.google.com",
            "www.scs.stanford.edu",
            "yahoo.com",
            "google.com",
           "youtube.com",
            "tradingview.com",
            "walla.co.il",
            "nasa.com",
            "medium.com",
            "www.scs.stanford.edu",
            "docs.google.com",
            "slides.google.com",
            "sheets.google.com",
            "podcasts.google.com",
            "gmail.google.com",
            "account.google.com",
        };
        for (string &str: hosts) {
            dns_client.query(DNS_TYPE_A, str);
            cout << endl << endl << endl;
        }

    }
}

int main(int argc, char** argv) {
//    namespace po = boost::program_options;
//
//    po::options_description desc("Allowed options");
//    desc.add_options()
//            ("help", "print tool use description")
//            ("path,p", po::value<vector<string>>()->multitoken(),
//             "the path along the onion network to use");
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, desc), vm);
//    po::notify(vm);
//
//    if (vm.count("help")) {
//        cout << desc << endl;
//        return 1;
//    }
//
//    if (!vm.count("path")) {
//        cout << desc << endl;
//        return 1;
//    }
//    vector<string> path = vm["path"].as<vector<string>>();
//
//    vector<ip4_addr> ip_path;
//    for (string& node : path) ip_path.push_back(convert_to_ip4_addr(node));
//
//    onion_network_node_main(ip_path);
    onion_network_node_main({convert_to_ip4_addr("10.100.102.31")});

}