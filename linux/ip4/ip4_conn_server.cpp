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

void ip4_conn_server::register_filter(int prot) {
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

ip4_conn_server::ip4_conn_server() {
    cout << "waiting" << endl;
}


int ip4_conn_server::recv_next_msg( void* data, int count) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

#define BUFF_LEN 256
    char buff[BUFF_LEN];
    memset(&buff, '\x00', BUFF_LEN);

    // just before receiving, make sure there is a file descriptor for this protocol
    if (!prot_handlers.count(getNextProt())) register_filter(getNextProt());

    int res = recvfrom(prot_handlers[getNextProt()].addit_data.fd,
                       buff, BUFF_LEN,
                       0,
                       (struct sockaddr *) &client_addr, &len);


    struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(buff);
//    port_handlers[prot].last_client = {ntohl(client_addr.sin_addr.s_addr)};
    prot_handlers[getNextProt()].last_client = {ntohl(ip_hdr->saddr)};

    char *packet_data = buff + sizeof(struct iphdr);

    int copy_cnt = std::min(res - (int) sizeof(struct iphdr), count);
    memcpy(data, packet_data, copy_cnt);
    return copy_cnt;

//    prot_handlers[prot].data = std::unique_ptr<char>(alloc);

}

int ip4_conn_server::send_next_msg( ip4_addr client, void *buff, int count) {
    struct sockaddr_in sock;
    memset(&sock, 0, sizeof(sock));
    sock.sin_addr.s_addr = htonl(client.raw);

    // just before sending, make sure there is a file descriptor for this protocol
    if (!prot_handlers.count(getNextProt())) register_filter(getNextProt());

    return sendto(prot_handlers[getNextProt()].addit_data.fd,
           buff, count,
           0,
           reinterpret_cast<const sockaddr *>(&sock), sizeof(sock));

}

