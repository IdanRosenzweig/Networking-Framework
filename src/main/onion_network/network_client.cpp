
#include "../../temp_utils/proxy/ip_proxy_client.h"

#include "../../temp_prot_stacks/udp_client_server/udp_client.h"
#include "../../temp_utils/dns_server_client/dns_client.h"
#include "../../temp_prot_stacks/icmp/icmp_connection_client.h"
#include "../../temp_utils/onion_network/onion_network_client.h"

#include "../../linux/virtual_if.h"
#include "../../linux/hardware.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;


void onion_network_node_main(const string& iface, const vector<ip4_addr>& path) {
    onion_network_client proxy(path, get_ip_addr_of_iface(iface), new network_layer_gateway(iface));

    char dev[6] = "virt0";
    linux_virtual_iface virt_iface(&proxy, dev);

    cout << "virtual interface \"" << dev << "\" is open. send your ip traffic through it" << endl << endl;

    while (true) {

    }

//     regular dns communication
    {
        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), convert_to_ip4_addr("10.100.102.18"), new interface_gateway("virt0"));

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
            cout << endl;
        }

    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")
            ("path,p", po::value<vector<string>>()->multitoken(),
             "the path along the onion network to use");

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

    if (!vm.count("path")) {
        cout << opts << endl;
        return 1;
    }
    vector<string> path = vm["path"].as<vector<string>>();

    vector<ip4_addr> ip_path;
    for (string& node : path) ip_path.push_back(convert_to_ip4_addr(node));

    onion_network_node_main(iface, ip_path);

}