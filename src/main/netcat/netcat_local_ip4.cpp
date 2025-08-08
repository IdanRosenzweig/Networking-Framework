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

#include "src/abstract/net_access/multi_net_access.h"

#include "src/protocols/ip4/ip4_protocol.h"

static constexpr ip_prot_values ip4_unsed_prot = static_cast<ip_prot_values>((uint16_t) 0xa0); 

void netcat_server_main(string const& iface_name) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    // shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#endif

    /* ip4 surface */
    shared_ptr<net_access> ip4_surface;
#ifdef BUILD_OS_LINUX
    ip4_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ip4_surface = vnet_net_access;
#endif

    struct my_unused_prot_surface_generator_listener : public net_access_generator_listener {
    public:
        void handle_new_net_access(shared_ptr<net_access> const& net_access) override {

            struct my_client_listener : public recv_listener_bytes {
            public:
                void handle_recv(vector<uint8_t> const& data) override {
                    cout << "a client sent: " << (char*) data.data() << endl;
                }
            };
            static auto handler = make_shared<my_client_listener>();
            net_access->set_recv_access(handler);

        }
    };
    ip4_protocol::connect_net_access_generator_listener(
        ip4_surface,
        get_ip4_addr_of_iface(iface_name).value(),
        ip4_unsed_prot,
        make_shared<my_unused_prot_surface_generator_listener>()
    );

    while (true) {
        string str;
        getline(cin, str);

        vector<uint8_t> data(str.data(), str.data() + str.size() + 1);

        // send to all clients
        // for (auto const& client : clients) {
        //     client.second->get_send_access()->send_data(data);
        // }
    }

}


void netcat_client_main(string const& iface_name, ip4_addr server_addr) {
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    // shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#endif

    /* ip4 surface */
    shared_ptr<net_access> ip4_surface;
#ifdef BUILD_OS_LINUX
    ip4_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ip4_surface = vnet_net_access;
#endif

    /* unused prot surface */
    auto unused_prot_surface = make_shared<ip4_protocol::net_access_single>(
        std::move(ip4_surface),
        server_addr, 
        get_ip4_addr_of_iface(iface_name).value(),
        ip4_unsed_prot
    );

    struct my_client : public recv_listener_bytes {
    public:
        void handle_recv(vector<uint8_t> const& data) override {
            cout << "the server sent: " << (char*) data.data() << endl;
        }
    };
    static auto client = make_shared<my_client>();
    unused_prot_surface->set_recv_access(client);
    

    while (true) {
        string str;
        getline(cin, str);

        // send to server
        unused_prot_surface->get_send_access()->send_data(vector<uint8_t>(str.data(), str.data() + str.size() + 1));
    }
}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("tool for sending and receiving raw text (on the local network)");
    opts.add_options()
        ("help,h", "print tool use description")
        ("iface", po::value<string>(), "linux interface to use")

        ("client", "use as client")
        ("server", "use as server")

        ("dest", po::value<string>(), "when used as client, this is the dest addr of the server")
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

    if (vm.count("server")) {
        netcat_server_main(iface);
        return 0;

    } else if (vm.count("client")) {
        if (!vm.count("dest")) {
            std::cout << opts << endl;
            return 1;
        }

        ip4_addr dest;
        if (auto tmp = str_to_ip4_addr(vm["dest"].as<string>()); tmp.has_value()) dest = tmp.value();
        else {
            cout << "invalid ip4 addr" << endl;
            return 1;
        }

        netcat_client_main(iface, dest);
        return 0;

    } else {
        std::cout << opts << endl;
        return 1;
    }

}