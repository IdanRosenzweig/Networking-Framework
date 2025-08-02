#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/virtual_net.h"
#include "lib/linux/hardware.h"

#include "src/tools/net_analyzer/net_analyzer.h"

void net_analyzer_main(string const& iface_name) {
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);

    shared_ptr<sniffer_recv> sniff_outgoing = make_shared<sniffer_recv>();
    iface->sniffing_set_outgoing(sniff_outgoing);

    shared_ptr<sniffer_recv> sniff_incoming = make_shared<sniffer_recv>();
    iface->sniffing_set_incoming(sniff_incoming);

    net_analyzer analyzer(sniff_outgoing, sniff_incoming);

    while (true) {
        sleep(1);
    }

}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("tool for sending and receiving raw text (on the local network)");
    opts.add_options()
        ("help,h", "print tool use description")
        ("iface", po::value<string>(), "linux interface to use")
    ;

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

    net_analyzer_main(iface);

}