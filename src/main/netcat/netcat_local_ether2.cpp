#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/virtual_net.h"
#include "lib/linux/hardware.h"

#include "src/abstract/network_access/multi_net_access.h"

#include "src/protocols/ether2/ethernet2_protocol.h"


static constexpr ethertype ethertype_unsed_prot = static_cast<ethertype>((uint16_t) 0x1234); 

void netcat_server_main(string const& iface_name) {
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);

    /* net access for ip4 broadcast over unused prot */
    auto net_access_generator = make_shared<ethernet2_protocol::net_access_generator_single>(std::move(vnet_net_access), get_mac_addr_of_iface(iface_name).value(), ethertype_unsed_prot);

    /* clients */
    struct my_client_listener : public basic_recv_listener<vector<uint8_t>> {
    public:
        void handle_recv(vector<uint8_t> const& data) override {
            cout << "a client sent: " << (char*) data.data() << endl;
        }
    };

    /* set the generator listener */
    struct my_generator_listener : public net_access_generator_listener {
    public:
        void handle_new_net_access(shared_ptr<net_access_bytes> const& net_access) override {
            // mac_addr addr = dynamic_cast<ethernet2_protocol::net_access_single*>(net_access.get())->src_addr;
            // cout << "got new net access from " << mac_addr_to_str(addr) << endl;

            static auto handler = make_shared<my_client_listener>();
            net_access->set_recv_access(handler);
        }

    };
    net_access_generator->set_net_access_generator_listener(make_shared<my_generator_listener>());

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


void netcat_client_main(string const& iface_name, mac_addr server_addr) {
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);

    /* single net access for ip4 */
    auto server_net_access = make_shared<ethernet2_protocol::net_access_single>(std::move(vnet_net_access), server_addr, get_mac_addr_of_iface(iface_name).value(), ethertype_unsed_prot);

    struct my_client : public basic_recv_listener<vector<uint8_t>> {
    public:
        void handle_recv(vector<uint8_t> const& data) override {
            cout << "the server sent: " << (char*) data.data() << endl;
        }
    };
    shared_ptr<my_client> client = make_shared<my_client>();
    server_net_access->set_recv_access(client);

    while (true) {
        string str;
        getline(cin, str);

        // send to server
        server_net_access->get_send_access()->send_data(vector<uint8_t>(str.data(), str.data() + str.size() + 1));
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

        mac_addr dest;
        if (auto tmp = str_to_mac_addr(vm["dest"].as<string>()); tmp.has_value()) dest = tmp.value();
        else {
            cout << "invalid mac addr" << endl;
            return 1;
        }

        netcat_client_main(iface, dest);
        return 0;

    } else {
        std::cout << opts << endl;
        return 1;
    }

}