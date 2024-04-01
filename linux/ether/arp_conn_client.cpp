#include "arp_conn_client.h"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <sys/ioctl.h>

using namespace std;

arp_conn_client::arp_conn_client() {

}

void arp_conn_client::init() {
//    fd = socket(AF_PACKET,
//                SOCK_RAW,
//                htons(ETH_P_ARP));
//    if (fd == -1) {
//        cerr << "can't open socket" << endl;
//        return;
//    }
//
//    mac_addr addr = get_my_mac_address(fd, "enp0s3");
//    printf("my mac is: %02x:%02x:%02x:%02x:%02x:%02x\n",
//           (unsigned int) addr.addr[0],
//           (unsigned int) addr.addr[1],
//           (unsigned int) addr.addr[2],
//           (unsigned int) addr.addr[3],
//           (unsigned int) addr.addr[4],
//           (unsigned int) addr.addr[5]
//    );

}

void arp_conn_client::finish() {
//    close(fd);
}

void arp_conn_client::spoof(const string &spoofed_mac, const string &ip, const string &device_mac) {

}

//void arp_conn_client::discover(const std::string ip) {

//
//#define PACKET_LEN 256
//    char packet[PACKET_LEN];
//    struct iphdr *iph = (struct iphdr *)packet;
//
//    char *data = packet + sizeof(struct iphdr);
////    strcpy(data, "Hello, this is raw data from the client.");
//    memcpy(data, buff, count);
//
//    iph->ihl = 5;
//    iph->version = 4;
//    iph->tos = 0;
//    iph->tot_len = sizeof(struct iphdr) + count;
//    iph->id = htonl(4444);
//    iph->frag_off = 0;
//    iph->ttl = 255;
//    iph->protocol = prot;
//    iph->check = 0;
//    iph->saddr = 0; // Set source IP to 0 for the kernel to fill it automatically
//    iph->daddr = dest_addr.sin_addr.s_addr;
//
//    iph->check = checksum((unsigned short *)packet, iph->tot_len >> 1);
//
//    return sendto(fd, packet, iph->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));


//#define PACKET_LEN 50
//    char packet[PACKET_LEN];
//
//    socklen_t sock_len = sizeof(dest_addr);
//
//    int res = recvfrom(fd, packet, PACKET_LEN, 0, (struct sockaddr *)&dest_addr, &sock_len);
//
//    char *data = packet + sizeof(struct iphdr);
//    int data_cnt = std::min((int) (res - sizeof(struct iphdr)), count);
//
//    memcpy(buff, data, data_cnt);
//    return data_cnt;

//}

