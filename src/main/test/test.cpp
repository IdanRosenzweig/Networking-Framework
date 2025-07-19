#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_network_access.h"

#include "src/protocols/ether/ethernet2_protocol.h"

int main() {
    /* network access in linux */
    string iface_name = "enp0s3";
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<network_access_bytes> iface_net_access = make_shared<linux_iface_network_access>(iface);

    /* ethernet 2 */
    ethernet2_protocol ether;

    // setup send
    ether.set_net_access(iface_net_access);

    // prepare parameters
    ether.next_source_addr = mac_addr{{0x11, 0x22, 0x33, 0x44, 0x55, 0x66}};
    ether.next_dest_addr = mac_addr{{0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc}};
    ether.next_protocol = ethertype::ip4;

    // send
    ether.send(vector<uint8_t>());

    // setup recv
    ether.listener_conds.push_back({
        [](shared_ptr<ethernet_header> const& eth_header) -> bool {
            cout << "called" << endl;
            return eth_header->ether_type == ethertype::ip4;
        },
        nullptr
    });

    /* bridge */
    // struct network_access_from_ether : public network_access_bytes {
    // private:
    //     shared_ptr<ethernet2_protocol> ethernet2;

    // public:
    //     void set_ethernet2(shared_ptr<ethernet2_protocol> const& _ethernet2) {
    //         ethernet2 = _ethernet2;

    //         struct my_recv : public encap_prot_listener<ethernet_header> {
    //             network_access_from_ether* par;

    //             my_recv(network_access_from_ether* par) : par(par) {}

    //             void handle_recv(pair<shared_ptr<ethernet_header>, vector<uint8_t>> const& data) {

    //             }
    //         };

    //         ethernet2->listener_prot[ethertype::arp] = make_shared<my_recv>(this);
    //     }


    // };




    sleep(10);

    /* done */
    cout << "done" << endl;
}
