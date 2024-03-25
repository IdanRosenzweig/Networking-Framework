#include "linux_client_ip4.h"
#include <iostream>
#include <netinet/ip.h>
using namespace std;

unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

std::unique_ptr<linux_cl_conn<ip4_conn>> linux_client_ip4::conn_to_host() {
    int fd = socket(AF_INET,
                    SOCK_RAW,
                    IPPROTO_RAW);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return nullptr;
    }

    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        return nullptr;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\x00', sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

//    cout << "ready" << endl;
//
//
//    cout << "sending data" << endl;
//#define PACKET_LEN 64
//    char packet[PACKET_LEN];
//    struct iphdr *iph = (struct iphdr *)packet;
//    char *data = packet + sizeof(struct iphdr);
//    strcpy(data, "Hello, this is raw data from the client.");
//
//    iph->ihl = 5;
//    iph->version = 4;
//    iph->tos = 0;
//    iph->tot_len = sizeof(struct iphdr) + strlen(data);
//    iph->id = htonl(4444);
//    iph->frag_off = 0;
//    iph->ttl = 211;
//    iph->protocol = IPPROTO_RAW;
//    iph->check = 0;
//    iph->saddr = 0; // Set source IP to 0 for the kernel to fill it automatically
//    iph->daddr = server_addr.sin_addr.s_addr;
//
//
//    iph->check = checksum((unsigned short *)packet, iph->tot_len >> 1);
//
//    cout << "sent " << sendto(fd, packet, iph->tot_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) << " bytes" << endl;

    auto *host = new linux_cl_conn<ip4_conn>;
    host->fd = fd; // my open fd
    host->addr = server_addr; // server's addr
    return std::unique_ptr<linux_cl_conn<ip4_conn>>(host);
}

linux_client_ip4::linux_client_ip4(const string &ip) : ip(ip) {

}
