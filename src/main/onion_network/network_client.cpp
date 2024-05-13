#include <iostream>

#include "../../temp_utils/proxy/network_layer/ip_proxy_client.h"

#include "../../temp_connections/udp_client_server/udp_client.h"
#include "../../temp_utils/dns_server_client/dns_client.h"
#include "../../temp_connections/icmp/icmp_connection_client.h"
#include "../../temp_utils/onion_network/client.h"

#include "../../linux/virtual_if.h"
#include <unistd.h>
#define MY_IP "10.100.102.18"


void onion_network_main() {
    client proxy({
//                         convert_to_ip4_addr("10.100.102.34"),
                         convert_to_ip4_addr("10.100.102.31"),
                 });

    char dev[6] = "virt0";
    linux_virtual_iface iface = create_virtual_iface_from_connection(&proxy, dev);

//    while (true) {
//
//    }

    // icmp
//    {
//        char *str = "172.217.22.46";
//        //    char* str = "google.com";
//
//        ip4_protocol ip_client;
//        icmp_protocol icmp_client;
//
//        // setup send flow
//        ip_client.gateway = &proxy;
//        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
//        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
//        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));
//
//        icmp_client.gateway = &ip_client;
//
//        // setup recv flow
//        proxy.add_listener(&ip_client);
//
//        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

//        // ping_util
//        icmp_client.ping_util();
//    }

//     regular dns communication
    {
//        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), &proxy);
        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"), new interface_gateway("virt0"));
//        dns_client dns_client(convert_to_ip4_addr("8.8.8.8"));

        // communicate
        vector<string> hosts = {
            "wiki.osdev.org",
            "www.wix.com",
            "docs.google.com",
            "www.scs.stanford.edu",
            "yahoo.com",
            "google.com",
           "youtube.com",
            "tradingview.com",
            "walla.co.il",
            "nasa.com",
            "medium.com",
            "www.scs.stanford.edu",
            "docs.google.com",
            "slides.google.com",
            "sheets.google.com",
            "podcasts.google.com",
            "gmail.google.com",
            "account.google.com",
        };
        for (string &str: hosts) {
            dns_client.query(DNS_TYPE_A, str);
            cout << endl << endl << endl;
        }
    }
}

int main() {
//    proxy_main();
    onion_network_main();
}

