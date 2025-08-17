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

#include "lib/hexdump.h"

#include "src/abstract/net_access/multi_net_access.h"

#include "src/protocols/ip4/ip4_protocol.h"
#include "src/protocols/ptp/ptp_protocol.h"

struct A {
    virtual ~A() = default;
    virtual void a_func() = 0;
};

struct B {
    virtual ~B() = default;
    virtual void b_func() = 0;
};

struct C : public A, public B {
};

struct my_impl : public C {
    void a_func() override {
        // something
    }
    void b_func() override {
        // something
    }
};

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

    /* ptp surface */
    shared_ptr<net_access> ptp_surface;
#ifdef BUILD_OS_LINUX
    ptp_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ptp_surface = vnet_net_access;
#endif
    multi_net_access multi_ptp_surface(std::move(ptp_surface));

    // a ptp node
    ptp_protocol::node ptp_node((shared_ptr<net_access_recv>) multi_ptp_surface.generate_net_access());

    ptp_node.my_addr = str_to_ptp_addr("33.44.55").value();

    struct my_data_surface_generator_listener : public net_access_generator_listener {
    public:
        void handle_new_net_access(shared_ptr<net_access> const& _net_access) override {

            struct my_client_listener : public recv_listener_bytes {
                weak_ptr<net_access> access;

            public:

                my_client_listener(weak_ptr<net_access> const& _access) : access(_access) {
                }

                void handle_recv(vector<uint8_t> const& data) override {
                    cout << "a client sent: " << (char*) data.data() << endl;

                    string response = "hello back from the server";
                    vector<uint8_t> bytes(response.data(), response.data() + response.size() + 1);
                    cout << "sending back: " << response << endl;
                    access.lock()->get_send_access()->send_data(bytes);
                }
            };
            _net_access->set_recv_access(make_shared<my_client_listener>(_net_access));

        }
    };
    // ptp_protocol::connect_net_access_generator_listener(
    //     ptp_surface,
    //     make_shared<my_data_surface_generator_listener>()
    // );
    // ptp_node.generator_listener = make_shared<my_data_surface_generator_listener>();

    auto net_access_recv_endpoint = ptp_node.get_recv_endpoint_end();
    ptp_protocol::connect_net_access_generator_listener(
        net_access_recv_endpoint,
        (shared_ptr<net_access_send>) multi_ptp_surface.generate_net_access(),
        make_shared<my_data_surface_generator_listener>()
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


void netcat_client_main(string const& iface_name, ptp_action_list const& action_list) {
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

    /* ptp surface */
    shared_ptr<net_access> ptp_surface;
#ifdef BUILD_OS_LINUX
    ptp_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ptp_surface = vnet_net_access;
#endif
    multi_net_access multi_ptp_surface(std::move(ptp_surface));

    // a ptp node
    ptp_protocol::node ptp_node((shared_ptr<net_access_recv>) multi_ptp_surface.generate_net_access());

    ptp_node.my_addr = str_to_ptp_addr("11.22").value();

    auto ptp_recv_endpoint = ptp_node.get_recv_endpoint_end();
    auto tmp = net_access_make_from_send_recv(multi_ptp_surface.generate_net_access(), std::move(ptp_recv_endpoint));

    /* data surface */
    auto data_surface = make_shared<ptp_protocol::net_access_single>(
        std::move(tmp),
        action_list
    );

    struct my_client : public recv_listener_bytes {
    public:
        void handle_recv(vector<uint8_t> const& data) override {
            cout << "the server sent: " << (char*) data.data() << endl;
        }
    };
    static auto client = make_shared<my_client>();
    data_surface->set_recv_access(client);
    

    while (true) {
        string str;
        getline(cin, str);

        // send to server
        data_surface->get_send_access()->send_data(vector<uint8_t>(str.data(), str.data() + str.size() + 1));
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

        ("dest", po::value<string>(), "when used as client, this is the action list to reach the server")
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

        ptp_action_list action_list;
        if (auto tmp = str_to_ptp_action_list(vm["dest"].as<string>()); tmp.has_value()) action_list = tmp.value();
        else {
            cout << "invalid action list" << endl;
            return 1;
        }

        netcat_client_main(iface, action_list);
        return 0;

    } else {
        std::cout << opts << endl;
        return 1;
    }

}