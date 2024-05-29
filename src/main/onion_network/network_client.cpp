
#include "../../temp_utils/proxy/ip_proxy_client.h"

#include "../../temp_prot_stacks/udp_client_server/udp_client.h"
#include "../../temp_utils/dns/dns_client.h"
#include "../../temp_prot_stacks/icmp/icmp_connection_client.h"
#include "../../temp_utils/onion_network/onion_network_client.h"
#include "../../temp_utils/ping/ping_util.h"

#include "../../linux/if/virtual/virtual_if.h"
#include "../../linux/if/hardware.h"

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;


void onion_network_node_main(const string &iface, const vector<ip4_addr> &path) {
    std::shared_ptr<iface_access_point> iface_access = make_shared<iface_access_point>(iface);

    onion_network_client proxy(path, get_ip_addr_of_iface(iface), new network_layer_gateway(iface_access));

    string new_iface = "virt0";
    linux_virtual_iface virt_iface(&proxy, new_iface);

    std::cout << "the new virtual interface \"" << new_iface << "\" is open. send your ip traffic through it, and receive ip traffic back"
         << endl << endl;

//    while (true) {
//
//    }
    std::shared_ptr<iface_access_point> new_iface_access = make_shared<iface_access_point>(new_iface);

    auto raw_if = new interface_gateway(new_iface_access);

    // dns queries
    {
        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), convert_to_ip4_addr("10.100.102.18"),
                              raw_if);

        // communicate
        vector<string> names = {
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
        for (string &str: names) {
            dns_client.query(DNS_TYPE_A, str);
            std::cout << endl;
            std::this_thread::sleep_for(100ms);
        }
    }


    // pinging
    {
        ping_util pinger(get_ip_addr_of_iface(iface), raw_if);

        // communicate
        vector<string> hosts = {
                "8.8.8.8",
                "142.251.142.206",
                "34.149.87.45",
                "13.226.2.85",
                "13.226.2.79",
                "172.217.22.14",
                "185.53.177.52",
                "162.159.153.4",
                "142.251.142.206",
                "162.159.152.4",
                "171.66.3.9",
                "172.217.22.46",

                "142.251.142.206"
        };

        std::cout << "starting to ping" << endl;
        pinger.count.set_next_choice(5);
        pinger.delay_interval.set_next_choice(10ms);
        for (string &str: hosts) {
            pinger.dest_ip.set_next_choice(convert_to_ip4_addr(str));
            pinger.ping_node();
            std::cout << endl;
            std::this_thread::sleep_for(100ms);
        }
    }

}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use to connect to the first node in the path")
            ("path,p", po::value<vector<string>>()->multitoken(),
             "the path along the onion network to use");

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

    if (!vm.count("path")) {
        std::cout << opts << endl;
        return 1;
    }
    vector<string> path = vm["path"].as<vector<string>>();

    vector<ip4_addr> ip_path;
    for (string &node: path) ip_path.push_back(convert_to_ip4_addr(node));

    onion_network_node_main(iface, ip_path);

}