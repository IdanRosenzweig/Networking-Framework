#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_network_access.h"

#include "src/protocols/ether/ethernet2_protocol.h"
#include "src/protocols/arp/arp_header.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/tools/arp_scanner/arp_scanner.h"

int main() {
    puts("starting");

    /* network access in linux */
    string iface_name = "enp0s3";
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<network_access_bytes> iface_net_access = make_shared<linux_iface_network_access>(iface);

    /* ethernet 2 */
    shared_ptr<ethernet2_protocol> ether = make_shared<ethernet2_protocol>();

    // setup send
    ether->set_net_access(iface_net_access);

    // prepare parameters
    ether->next_source_addr = mac_addr{{0x11, 0x22, 0x33, 0x44, 0x55, 0x66}};
    ether->next_dest_addr = mac_addr{{0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc}};
    ether->next_protocol = ethertype::ip4;

    // send
    // ether->send(vector<uint8_t>());

    // setup recv
    // ether->listener_conds.push_back({
    //     [](shared_ptr<ethernet_header> const& eth_header) -> bool {
    //         cout << "called" << endl;
    //         return eth_header->ether_type == ethertype::ip4;
    //     },
    //     nullptr
    // });

    /* bridge */
    // network access to receive arp from any mac addr, and always send to broadcast
    struct network_access_arp_from_ether : public network_access_bytes {
    public:
        shared_ptr<ethernet2_protocol> ethernet2;

        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

    public:
        void set_ethernet2(shared_ptr<ethernet2_protocol> const& _ethernet2) {
            ethernet2 = _ethernet2;

            struct my_recv : public encap_prot_listener<ethernet_header> {
                network_access_arp_from_ether* par;
                my_recv(network_access_arp_from_ether* par) : par(par) {}

                void handle_recv(pair<shared_ptr<ethernet_header>, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    par->recv_access->handle_recv(data.second);
                }
            };

            puts("making recv access");

            ethernet2->listener_prot[ethertype::arp] = make_shared<my_recv>(this);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                network_access_arp_from_ether* par;
                my_send(network_access_arp_from_ether* par) : par(par) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    puts("preparing params");
                    par->ethernet2->next_source_addr = mac_addr{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
                    par->ethernet2->next_dest_addr = mac_addr_broadcast;
                    par->ethernet2->next_protocol = ethertype::arp;

                    puts("sending (arp_from_ether bridge)");
                    par->ethernet2->send(data);

                    return err_t::OK;// todo fix this
                }
            };

            puts("making send access");

            send_access = make_shared<my_send>(this);

            puts("done");
        }

        shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
            recv_access = recv;
        }

    };
    
    shared_ptr<network_access_arp_from_ether> ether_to_arp_bridge = make_shared<network_access_arp_from_ether>();
    ether_to_arp_bridge->set_ethernet2(ether);


    puts("scanning");
    mac_addr res = arp_scanner::search_for_mac_addr(ether_to_arp_bridge, str_to_ip4_addr("10.0.2.2").value(), mac_addr_empty, ip4_addr_empty);
    printf("res: %s\n", mac_addr_to_str(res).c_str());

    puts("scanning entire subnet");
    auto entries = arp_scanner::scan_entire_subnet(ether_to_arp_bridge, str_to_ip4_subnet_mask("10.0.2.0/24").value(), mac_addr_empty, ip4_addr_empty);
    for (auto const& entry : entries) {
        printf("%s is %s\n", ip4_addr_to_str(entry.first).c_str(), mac_addr_to_str(entry.second).c_str());
    }

    /* done */
    cout << "done" << endl;
}
