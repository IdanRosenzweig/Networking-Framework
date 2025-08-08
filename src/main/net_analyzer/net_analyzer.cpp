#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#ifdef BUILD_OS_LINUX
#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/virtual_net.h"
#include "lib/linux/hardware.h"
#elifdef BUILD_OS_MACOS
#include "lib/macos/macos_iface.h"
#include "lib/macos/macos_iface_net_access.h"
#include "lib/macos/hardware.h"
#include "lib/macos/virtual_net.h"
#endif

#include "src/tools/net_analyzer/net_analyzer.h"

void net_analyzer_main(string const& iface_name) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
#endif

    // create the sniff, and attach it
    shared_ptr<sniffer_recv> sniff = make_shared<sniffer_recv>();
#ifdef BUILD_OS_LINUX
    iface->set_sniffing(sniff);
#elifdef BUILD_OS_MACOS
    iface->set_sniffing(sniff);
#endif

    // create the net anazlyer based on the sniffer
    net_analyzer analyzer(sniff);

    while (true) {
        sleep(100000);
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