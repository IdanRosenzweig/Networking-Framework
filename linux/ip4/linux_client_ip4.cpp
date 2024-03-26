#include "linux_client_ip4.h"
#include <iostream>
#include <netinet/ip.h>
#include <cstring>

using namespace std;

unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

linux_client_ip4::linux_client_ip4(const string &ip) : ip(ip) {

}

void linux_client_ip4::init() {
    fd = socket(AF_INET,
                    SOCK_RAW,
                    IPPROTO_RAW);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return;
    }

    int one = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());



}

void linux_client_ip4::finish() {
    close(fd);
    fd = -1;
}

int linux_client_ip4::send_encapsulated_data(void *buff, int count) {
    cout << "ready" << endl;
    cout << "sending data" << endl;
#define PACKET_LEN 64
    char packet[PACKET_LEN];
    struct iphdr *iph = (struct iphdr *)packet;
    char *data = packet + sizeof(struct iphdr);
    strcpy(data, "Hello, this is raw data from the client.");

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + strlen(data);
    iph->id = htonl(4444);
    iph->frag_off = 0;
    iph->ttl = 211;
    iph->protocol = IPPROTO_RAW;
    iph->check = 0;
    iph->saddr = 0; // Set source IP to 0 for the kernel to fill it automatically
    iph->daddr = addr.sin_addr.s_addr;

    iph->check = checksum((unsigned short *)packet, iph->tot_len >> 1);

    int cnt = sendto(fd, packet, iph->tot_len, 0, (struct sockaddr *)&addr, sizeof(addr));
    cout << "sent " << cnt << " bytes" << endl;
    return cnt;
}

int linux_client_ip4::recv_encapsulated_data(void *buff, int count) {
    return 0;
}
