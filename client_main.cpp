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
#include <linux/if.h>
#include <sys/fcntl.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>


//#define IP "127.0.0.1"
#define IP "10.100.102.18"
#define PORT 4444


int udp_main() {

    ethernet_conn_client ether_client;

    ip4_conn_client ip_client(IP);
    ip_client.ether_client = &ether_client;

    udp_conn_client client(PORT, 1212);
    client.ip_client = &ip_client;

        std::cout << "sending data" << std::endl;
        char *msg = "first";
        int len = strlen(msg);
        cout << "sent " << client.send_data(msg, len) << endl;


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
            "www.scs.stanford.edu",
    };

    ethernet_conn_client ether_client;

    ip4_conn_client ip_client("8.8.8.8");
    ip_client.ether_client = &ether_client;

    udp_conn_client udp_client(DNS_PORT, 1212);
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

    ethernet_conn_client ether_client;

    ip4_conn_client ip_client(str);
    ip_client.ether_client = &ether_client;

    icmp_conn_client icmp_client;
    icmp_client.ip_client = &ip_client;

    icmp_client.ping();
}

int arp_main() {

    ethernet_conn_client ether_client;

    arp_conn_client arp_client;
    arp_client.ether_client = &ether_client;


//    mac_addr res = arp_client.search_for_device("10.100.102.3");
//    print_mac(res);
    string victim = "10.100.102.2";
    vector<pair<mac_addr, string>> victim_list = {
//            {arp_client.search_for_device(victim),victim}
//            {{0x20, 0x7b, 0xd2, 0xaf, 0xdb, 0xc9}, victim}
    };
    arp_client.spoof_as_device("10.100.102.1", // router
                               victim_list);

}

int tap_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        return fd;
    }

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev)
    {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        close(fd);
        return err;
    }
//    strncpy(dev, ifr.ifr_name, IFNAMSIZ);
    return fd;
}

void tunnel_main() {
    ip4_conn_client ip_client(IP);

    udp_conn_client udp_client(PORT, 1212);
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


int main() {


//    udp_main();
    tcp_main();
//    dns_main();
//    icmp_main();
//    arp_main();
//    tunnel_main();

//    system("sudo /home/idan/CLionProjects/ServerClient/network_config.sh");


    return (0);
}


