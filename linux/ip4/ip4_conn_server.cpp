#include "ip4_conn_server.h"
#include <iostream>
#include <linux/if_ether.h>
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

void parse_packet(unsigned char *packet, ssize_t packet_size) {
    struct iphdr *iph = (struct iphdr *) packet;
    unsigned char *data = packet + iph->ihl * 4; // Skip IP header

    printf("Received packet with source IP: %s\n", inet_ntoa(*(struct in_addr *) &iph->saddr));
    printf("Payload data: %s\n", data);
}

ip4_conn_server::ip4_conn_server() {
    cout << "waiting" << endl;
}

void ip4_conn_server::register_handler(int prot) {
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


void ip4_conn_server::recv_prot_next_msg(int prot) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

#define BUFF_LEN 256
    char buff[BUFF_LEN];
    memset(&buff, '\x00', BUFF_LEN);
    int res = recvfrom(prot_handlers[prot].addit_data.fd,
                       buff, BUFF_LEN,
                       0,
                       (struct sockaddr *) &client_addr, &len);

    struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(buff);
//    prot_handlers[prot].last_client = {ntohl(client_addr.sin_addr.s_addr)};
    prot_handlers[prot].last_client = {ntohl(ip_hdr->saddr)};

    char *data = buff + sizeof(struct iphdr);

    char *alloc = new char[BUFF_LEN - sizeof(struct iphdr)];
    memcpy(alloc, data, BUFF_LEN - sizeof(struct iphdr));

    prot_handlers[prot].data = std::unique_ptr<char>(alloc);

//    prot_handlers[prot].handler->recv_data(data);

}

void ip4_conn_server::send_next_prot_msg(int prot, ip4_addr client, void *buff, int count) {
    struct sockaddr_in sock;
    memset(&sock, 0, sizeof(sock));
    sock.sin_addr.s_addr = htonl(client.raw);

    sendto(prot_handlers[prot].addit_data.fd,
           buff, count,
           0,
           reinterpret_cast<const sockaddr *>(&sock), sizeof(sock));

}

