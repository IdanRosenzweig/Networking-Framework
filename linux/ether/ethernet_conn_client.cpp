#include "ethernet_conn_client.h"

#include <iostream>
#include <netinet/ip.h>
#include <cstring>

using namespace std;

ethernet_conn_client::ethernet_conn_client(const string &ip, int protocol) : ip(ip), prot(protocol) {

}

void ethernet_conn_client::init() {
    fd = socket(AF_INET,
                SOCK_RAW,
                prot);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return;
    }

    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        return;
    }

    dest_addr.sin_family = AF_INET;
//    dest_addr.sin_port = htons(4444); // todo uncomment?
    dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());

}

void ethernet_conn_client::finish() {
    close(fd);
}

int ethernet_conn_client::send_encapsulated_data(void *buff, int count) {

#define PACKET_LEN 256
    char packet[PACKET_LEN];
    struct iphdr *iph = (struct iphdr *)packet;

    char *data = packet + sizeof(struct iphdr);
//    strcpy(data, "Hello, this is raw data from the client.");
    memcpy(data, buff, count);

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + count;
    iph->id = htonl(4444);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = prot;
    iph->check = 0;
    iph->saddr = 0; // Set source IP to 0 for the kernel to fill it automatically
    iph->daddr = dest_addr.sin_addr.s_addr;

    iph->check = checksum((unsigned short *)packet, iph->tot_len >> 1);

    return sendto(fd, packet, iph->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

int ethernet_conn_client::recv_encapsulated_data(void *buff, int count) {
#define PACKET_LEN 50
    char packet[PACKET_LEN];

    socklen_t sock_len = sizeof(dest_addr);

    int res = recvfrom(fd, packet, PACKET_LEN, 0, (struct sockaddr *)&dest_addr, &sock_len);

    char *data = packet + sizeof(struct iphdr);
    int data_cnt = std::min((int) (res - sizeof(struct iphdr)), count);

    memcpy(buff, data, data_cnt);
    return data_cnt;
}
