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

#include "src/abstract/receiving/recv_listener.h"

#include "src/abstract/net_access/net_access.h"
#include "src/abstract/net_access/multi_net_access.h"

#include "src/protocols/ether2/ethernet2_protocol.h"

#include "src/protocols/ip4/ip4_protocol.h"
#include "src/protocols/ip4/ip4_routing_table.h"

#include "src/protocols/udp/udp_protocol.h"

#include "src/protocols/dns/dns.h"
#include "src/protocols/dns/dns_database.h"
#include "src/tools/dns/dns_tool.h"

#include "lib/hexdump.h"

dns_database* global_database;

void dns_server_main(string const& iface_name, dns_database* database) {
    global_database = database;
    
#ifdef BUILD_OS_LINUX
    /* network access in linux */
    // shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    // shared_ptr<net_access> iface_net_access = make_shared<linux_iface_net_access>(iface);
    /* virtual net access in linux */
    shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    vnet_net_access->connect(iface_name);
#elifdef BUILD_OS_MACOS
    /* network access in macos */
    shared_ptr<macos_iface> iface = make_shared<macos_iface>(iface_name);
    shared_ptr<net_access> iface_net_access = make_shared<macos_iface_net_access>(iface);
    /* virtual net access in linux */
    // shared_ptr<virtual_net> vnet_net_access = make_shared<virtual_net>();
    // vnet_net_access->connect(iface_name);
#endif

    /* ether2 surface */
    shared_ptr<net_access> ether2_surface;
#ifdef BUILD_OS_LINUX
    ether2_surface = vnet_net_access;
#elifdef BUILD_OS_MACOS
    ether2_surface = iface_net_access;
#endif
    multi_net_access ether2_multi_surface(std::move(ether2_surface));

    // my routing table for the ip4 network 
    ip4_routing_table routing_table;
    routing_table.set_default_gateway(
        make_shared<ethernet2_protocol::net_access_single>(
            ether2_multi_surface.generate_net_access(),
            str_to_mac_addr("c4:eb:42:ed:c5:b7").value(),
            get_mac_addr_of_iface(iface_name).value(),
            ethertype::ip4
        )
    );
    auto routed_ip4_send_surface = ip4_protocol::create_send_access_from_routing_table(std::move(routing_table));

    shared_ptr<net_access> ip4_recv_surface = make_shared<ethernet2_protocol::net_access_broadcast>(
        ether2_multi_surface.generate_net_access(),
        get_mac_addr_of_iface(iface_name).value(),
        ethertype::ip4
    );

    shared_ptr<net_access> ip4_surface = net_access_save_send_access(std::move(ip4_recv_surface), std::move(routed_ip4_send_surface)); // can send and receive ip packets through this net_access

    struct my_udp_surface_generator_listener : public net_access_generator_listener {
    public:
        void handle_new_net_access(shared_ptr<net_access> const& udp_surface) override {

            struct my_dns_surface_generator_listener : public net_access_generator_listener {
            public:
                void handle_new_net_access(shared_ptr<net_access> const& dns_surface) override {

                    struct my_dns_handler : public recv_listener<struct dns_message> {
                    public:
                        shared_ptr<net_access> dns_surface; // todo weak_ptr?
                        my_dns_handler(shared_ptr<net_access> _dns_surface) : dns_surface(_dns_surface) {
                        }

                    protected:
                        void handle_recv(struct dns_message const& msg) override {
                            cout << "received dns msg:" << endl;
                            print_dns_message(&msg, true, false);
                            cout << endl << endl;

                            struct dns_message reply = dns_tool::handle_query(global_database, &msg);

                            cout << "sending answer:" << endl;
                            print_dns_message(&reply, false, true);
                            cout << endl << endl;

                            uint8_t tmp_buff[0x1000];
                            auto tmp_buff_sz = write_in_network_order(tmp_buff, &reply);

                            dns_surface->get_send_access()->send_data(vector<uint8_t>(tmp_buff, tmp_buff + tmp_buff_sz));
                        }
                    };
                    dns_protocol::connect_recv(dns_surface, make_shared<my_dns_handler>(dns_surface));

                }
            };
            udp_protocol::connect_net_access_generator_listener(
                std::move(shared_ptr<net_access>(udp_surface)),
                udp_port(DNS_SERVER_PORT),
                make_shared<my_dns_surface_generator_listener>()
            );

        }

    };
    ip4_protocol::connect_net_access_generator_listener(
        ip4_surface,
        get_ip4_addr_of_iface(iface_name).value(),
        ip_prot_values::udp,
        make_shared<my_udp_surface_generator_listener>()
    );

    while (true) {
        sleep(10000);
    }

}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("start dns sever daemon");
    opts.add_options()
        ("help,h", "print tool use description")

        ("iface", po::value<string>(), "linux interface to use")

        ("db_a", po::value<string>(), "path to records (a) database file")
        ("db_mx", po::value<string>(), "path to records (mx) database file")
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

    
    dns_database database;

    // records type (a)
    if (vm.count("db_a")) {
        string db_path = vm["db_a"].as<string>();

        for (auto& [name, ip_addr] : dns_parse_records_a(db_path))
            database.add_record_a(name, ip_addr);
    }

    // records type (mx)
    if (vm.count("db_mx")) {
        string db_path = vm["db_mx"].as<string>();

        for (auto& [name, res] : dns_parse_records_mx(db_path))
            database.add_record_mx(name, res);
    }

    dns_server_main(iface, &database);







//     dns_database database;
//     database.add_record_a("my_name.local", str_to_ip4_addr("12.3.4.5").value());
//     database.add_record_a("my_name2.net", str_to_ip4_addr("16.3.4.5").value());


//     struct dns_message request;
    
//     request.header.id = 123;
//     request.header.qr = 0; // this is a request
//     request.header.opcode = 0; // standard
//     request.header.aa = 0;
//     request.header.tc = 0;
//     request.header.rd = 1; // no recursion
//     request.header.ra = 0;
// //    header.z = 0;
// //    header.ad = 0;
// //    header.cd = 0;
//     request.header.rcode = 0;
//     request.header.query_count = 1;
//     request.header.ans_count = 0;
//     request.header.auth_count = 0;
//     request.header.add_count = 0;

//     struct dns_query query;
//     string key = "my_name2.net";
//     query.q_name = vector<uint8_t>(key.data(), key.data() + key.size() + 1);
//     query.q_type = dns_record_type::a; // the type
//     query.q_class = 1; // the class
//     request.queries.push_back(query);

//     auto reply = dns_tool::handle_query(&database, &request);

//     print_dns_message(&reply, true, true);
}