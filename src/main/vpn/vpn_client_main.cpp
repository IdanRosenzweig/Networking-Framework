#include "../../linux/interface_gateway.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/icmp/icmp_protocol.h"

#include "../../temp_utils/vpn/vpn_client.h"
#include "../../linux/virtual_if.h"

#include <iostream>
using namespace std;

void vpn_main() {
    string vpn_server = "10.100.102.31";
    vpn_client vpn(convert_to_ip4_addr(vpn_server));

    char dev[6] = "virt0";
    linux_virtual_iface iface(&vpn, dev);

//    while (true) {
//
//    }

    // icmp
    {
//        sleep(4);
        char *str = "172.217.22.46";
        //    char* str = "google.com";

        interface_gateway dataLinkLayerGateway(dev);
        ethernet_protocol ether_client;
        ip4_protocol ip_client;
        icmp_protocol icmp_client;

        // setup send flow
        ether_client.gateway = &dataLinkLayerGateway;
        ether_client.next_protocol.set_next_choice(htons(ETH_P_IP));
        ether_client.next_dest_addr.set_next_choice(
                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
        );
//        ether_client.next_source_addr.set_next_choice(get_my_mac_address("enp0s3"));
        ether_client.next_source_addr.set_next_choice(mac_addr{0x80,0x00,0x27,0x29,0x04,0x44});

        ip_client.gateway = &ether_client;
        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
//        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));
        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));

        icmp_client.gateway = &ip_client;

        // setup recv flow
        dataLinkLayerGateway.add_listener(&ether_client);

        ether_client.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip_client);

        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

        // ping_util
        icmp_client.ping();
    }

//        regular dns communication
//    {
//        ethernet_protocol ether_prot;
//        ip4_protocol ip_prot;
//        bs_emp udp_prot;
//        dns_client dns_client;
//
//        // setup send flow
//        ether_prot.gateway = &vpn;
//        ether_prot.next_protocol.set_next_choice(htons(ETH_P_IP));
//        ether_prot.next_dest_addr.set_next_choice(
//                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
//        );
////        ether_prot.next_source_addr.set_next_choice(get_my_mac_address("enp0s3"));
//        ether_prot.next_source_addr.set_next_choice(mac_addr{0x80,0x00,0x27,0x29,0x04,0x44});
//
//        ip_prot.gateway = &ether_prot;
//        ip_prot.next_protocol.set_next_choice(IPPROTO_UDP);
//        ip_prot.next_dest_addr.set_next_choice(convert_to_ip4_addr("8.8.8.8"));
//        ip_prot.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));
//
//        udp_prot.gateway = &ip_prot;
//        udp_prot.next_source_point.set_next_choice(4545);
//        udp_prot.next_dest_point.set_next_choice(DNS_SERVER_PORT);
//
//        dns_client.gateway = &udp_prot;
//
//        // setup recv flow
//        vpn.add_listener(&ether_prot);
//
//        ether_prot.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip_prot);
//
//        ip_prot.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_prot);
//
//        udp_prot.port_handlers.assign_to_key(4545, &dns_client);
//
//        // communicate
//        // send msg
//        vector<string> hosts = {
//                "wiki.osdev.org",
//                "www.wix.com",
//                "docs.google.com",
//                "www.scs.stanford.edu",
//                "yahho.com",
//                "google.com",
//                "youtube.com",
//                "tradingview.com",
//                "walla.co.il",
//                "nasa.com",
//                "medium.com",
//                "www.scs.stanford.edu",
//        };
//
//        for (string& str : hosts)
//            dns_client.query(str);
//
//    }

}


int main() {
    vpn_main();
    return 0;
}
