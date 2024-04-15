#include <iostream>
#include <vector>
#include <linux/if_ether.h>
#include <pcap/pcap.h>
#include <thread>
#include <net/if.h>
#include <sys/ioctl.h>

#include "linux/tcp/tcp_conn_client.h"
#include "protocols/udp/udp_conn_client.h"
#include "protocols/dns/dns_client.h"
#include "protocols/ip4/ip4_conn_client.h"
#include "protocols/icmp/icmp_conn_client.h"
#include "protocols/ether/ethernet_conn_client.h"
#include "protocols/arp//arp_conn_client.h"

#include "linux/data_link_layer/data_link_layer_gateway.h"
#include "linux/network_layer_gateway/network_layer_gateway_client.h"
#include "abstract/basic_recv_conn.h"
#include "protocols/udp/udp_conn_client.h"
#include "linux/ip_level_proxy/ip_proxy_client.h"

//#define IP "127.0.0.1"
#define IP "10.100.102.18"
#define PORT 4444

class udp_client_system : public basic_recv_conn {
public:
    basic_send_conn *gateway;

    void print() {
        msg_protocol_stack msg = std::move(recv_data());
        cout << "server: " << (char *) (msg.msg.data.get() + msg.curr_offset) << endl;
    }

    void send(string str) {
        gateway->send_data((void *) str.c_str(), str.size());
    }
};


int udp_main() {
    udp_client_system udpAppClient;

    // protocol stack
    network_layer_gateway_client networkLayerGateway; // instead of providing the router's mac address, this calls wraps it for us

    ip4_conn_client ip_client;
    udp_conn_client udp_client;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr(IP));

    udp_client.gateway = &ip_client;
    udp_client.my_port.set_next_choice(1212);
    udp_client.next_external_port.set_next_choice(PORT);

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.prot_handlers[IPPROTO_UDP] = &udp_client;

    // connect app to udp client
    udpAppClient.gateway = &udp_client; // send
    udp_client.port_handlers[1212] = &udpAppClient;

    // communicate
    for (int i = 0; i < 4; i++) {
        udpAppClient.send("clint");
        udpAppClient.print();
    }

    cout << endl;

//    while (true) {
//
//    }

    return 0;
}

int tcp_main() {
    std::cout << "Hello, World!" << std::endl;

    tcp_conn_client client(IP, PORT);
    client.conn();

    std::cout << "sending data" << std::endl;
    char *msg = "hello";
    cout << "sent " << client.send_data(msg, strlen(msg)) << " bytes" << endl;
    std::cout << "data sent" << std::endl;

    char buff[6];
    memset(buff, '\x00', 6);
    client.recv_data(buff, 5);
    cout << "server: " << buff << endl;


    std::cout << "sending data" << std::endl;
    char *msg2 = "secnd";
    cout << "sent " << client.send_data(msg2, strlen(msg2)) << " bytes" << endl;
    std::cout << "data sent" << std::endl;

    char buff2[6];
    memset(buff2, '\x00', 6);
    client.recv_data(buff2, 5);
    cout << "server: " << buff2 << endl;

    while (true) {

    }

    return 0;
}

namespace dns {

    int dns_main() {
        vector<string> hosts = {
                "wiki.osdev.org",
                "www.wix.com",
                "docs.google.com",
                "www.scs.stanford.edu",
                "yahho.com",
                "google.com",
                "youtube.com",
                "tradingview.com",
                "walla.co.il",
                "nasa.com",
                "medium.com",
                "www.scs.stanford.edu",
        };

        // protocol stack
        network_layer_gateway_client networkLayerGateway;

        ip4_conn_client ip_client;
        udp_conn_client udp_client;
        dns_client dns_client;

        // setup send flow
        ip_client.gateway = &networkLayerGateway;
        ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr("8.8.8.8"));
        ip_client.next_protocol.set_next_choice(IPPROTO_UDP);

        udp_client.gateway = &ip_client;
        udp_client.my_port.set_next_choice(1212);
        udp_client.next_external_port.set_next_choice(DNS_PORT);

        dns_client.gateway = &udp_client;

        // setup recv flow
        networkLayerGateway.add_listener(&ip_client);

        ip_client.prot_handlers[IPPROTO_UDP] = &udp_client;

        udp_client.port_handlers[1212] = &dns_client;

        // communicate
        for (string &str: hosts) {
            dns_client.query(str);
        }

        while (true) {}

        return 0;
    }
}
int icmp_main() {
    char *str = "172.217.22.46";
//    char* str = "google.com";

    // protocol stack
    network_layer_gateway_client networkLayerGateway;

    ip4_conn_client ip_client;
    icmp_conn_client icmp_client;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr(str));
    ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);

    icmp_client.gateway = &ip_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.prot_handlers[IPPROTO_ICMP] = &icmp_client;

    // ping
    icmp_client.ping();
}

//int arp_main() {
//    data_link_layer_gateway data_link_gateway;
//
//    ethernet_conn_client ether_client;
//    ether_client.gateway = &data_link_gateway;
//    ether_client.next_external_protocol.set_next_choice(htons(ETH_P_ARP));
//
//    arp_conn_client arp_client;
//    arp_client.ether_client = &ether_client;
//
//
////    mac_addr res = arp_client.search_for_device("10.100.102.27");
////    print_mac(res);
//    string victim = "10.100.102.28";
//    vector<pair<mac_addr, string>> victim_list = {
//            {arp_client.search_for_device(victim), victim}
//    };
//    arp_client.spoof_as_device("10.100.102.1", // router
//                               victim_list, true);
//
//}

void proxy_main() {

    ip_proxy_client proxy;

    // regular dns communication
//    {
//
//        ip4_conn_client ip_client;
//        udp_conn_client udp_client;
//        dns_client dns_client;
//
//        // setup send flow
//        ip_client.gateway = &proxy;
//        ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr("8.8.8.8"));
//        ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
//
//        udp_client.gateway = &ip_client;
//        udp_client.my_port.set_next_choice(4545);
//        udp_client.next_external_port.set_next_choice(DNS_PORT);
//
//        dns_client.gateway = &udp_client;
//
//        // setup recv flow
//        proxy.add_listener(&ip_client);
//
//        ip_client.prot_handlers[IPPROTO_UDP] = &udp_client;
//
//        udp_client.port_handlers[4545] = &dns_client;
//
//        // communicate
//        // send data
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


    // icmp
    {
        char *str = "172.217.22.46";
    //    char* str = "google.com";

        ip4_conn_client ip_client;
        icmp_conn_client icmp_client;

        // setup send flow
        ip_client.gateway = &proxy;
        ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr(str));
        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);

        icmp_client.gateway = &ip_client;

        // setup recv flow
        proxy.add_listener(&ip_client);

        ip_client.prot_handlers[IPPROTO_ICMP] = &icmp_client;

        // ping
        icmp_client.ping();
    }
}

int main() {

//    udp_main();
//    tcp_main();
//    dns::dns_main();
//    icmp_main();
//    arp_main()
    proxy_main();

    return (0);
}

