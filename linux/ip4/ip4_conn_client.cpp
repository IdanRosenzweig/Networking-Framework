#include "ip4_conn_client.h"
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

ip4_conn_client::ip4_conn_client(const string &ip) : ip(ip) {
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

void ip4_conn_client::init() {
//    fd = socket(AF_INET,
//                    SOCK_RAW,
//                    prot);
//    if (fd == -1) {
//        cerr << "can't open socket" << endl;
//        return;
//    }
//
//    int one = 1;
//    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
//        perror("setsockopt");
//        return;
//    }
//

}

void ip4_conn_client::finish() {
//    close(fd);
}

//int ip4_conn_client::send_encapsulated_data(void *buff, int count) {
//
//#define BUFF_LEN 256
//    char packet[BUFF_LEN];
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
//}

//int ip4_conn_client::recv_encapsulated_data(void *buff, int count) {
//#define PACKET_LEN 50
//    char packet[BUFF_LEN];
//
//    socklen_t sock_len = sizeof(dest_addr);
//
//    int res = recvfrom(fd, packet, BUFF_LEN, 0, (struct sockaddr *)&dest_addr, &sock_len);
//
//    char *data = packet + sizeof(struct iphdr);
//    int data_cnt = std::min((int) (res - sizeof(struct iphdr)), count);
//
//    memcpy(buff, data, data_cnt);
//    return data_cnt;
//}
//
//


void ip4_conn_client::register_handler(int prot) {
    int fd = socket(
            AF_INET,
            SOCK_RAW,
            prot
    );
    if (fd == -1) {
        cerr << "socket err" << endl;
        printf("errno: %d\n", errno);
        throw;
    }

    prot_handlers[prot].addit_data = {fd};

}

int ip4_conn_client::recv_prot_next_msg(int prot, void* data, int count) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

#define BUFF_LEN 512
    char buff[BUFF_LEN];
    memset(&buff, '\x00', BUFF_LEN);
    int res = recvfrom(prot_handlers[prot].addit_data.fd,
                       buff, BUFF_LEN,
                       0,
                       (struct sockaddr *) &client_addr, &len);

    struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(buff);

    char *packet_data = buff + sizeof(struct iphdr);

    int copy_cnt = std::min(BUFF_LEN - (int) sizeof(struct iphdr), count);
    memcpy(data, packet_data, copy_cnt);
    return copy_cnt;

}

int ip4_conn_client::send_next_prot_msg(int prot, void *buff, int count) {
    return sendto(prot_handlers[prot].addit_data.fd,
           buff, count,
           0,
           reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));

}