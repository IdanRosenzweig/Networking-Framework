#include "ip4_conn_server.h"
#include <iostream>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include "checksum.h"

using namespace std;

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

    struct sockaddr_in sock;
    memset(&sock, 0, sizeof(sockaddr_in));
    sock.sin_port = htons(4444);
    if (bind(fd, (const sockaddr *) &sock, sizeof(sock)) == -1) {
        cout << "error binding" << endl;
        return;
    }
//    char *INTERFACE_NAME = "tap10";
//    if (setsockopt(fd,
//                   SOL_SOCKET, SO_BINDTODEVICE,
//                   INTERFACE_NAME, strlen(INTERFACE_NAME)) < 0) {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }

    prot_handlers[prot].addit_data = {fd};

}

ip4_conn_server::ip4_conn_server() {
    cout << "waiting" << endl;
}


int ip4_conn_server::recv_next_msg( void* data, int count) {
//    struct sockaddr_in client_addr;
//    socklen_t len = sizeof(client_addr);
//
//#define BUFF_LEN 256
//    char buff[BUFF_LEN];
//    memset(buff, '\x00', BUFF_LEN);
//
//    // just before receiving, make sure there is a file descriptor for this protocol
//    if (!prot_handlers.count(getNextProt())) register_filter(getNextProt());
//
//    int res = recvfrom(prot_handlers[getNextProt()].addit_data.fd,
//                       buff, BUFF_LEN,
//                       0,
//                       (struct sockaddr *) &client_addr, &len);
//
//
//    struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(buff);
////    port_handlers[prot].last_client = {ntohl(client_addr.sin_addr.s_addr)};
//    prot_handlers[getNextProt()].last_client = {ntohl(ip_hdr->saddr)};
//
//    char *packet_data = buff + sizeof(struct iphdr);
//
//    int copy_cnt = std::min(res - (int) sizeof(struct iphdr), count);
//    memcpy(data, packet_data, copy_cnt);
//    return copy_cnt;

#define BUFF_LEN 512
    char buff[BUFF_LEN];
    while (protocolQueue.prots[getNextProt()].empty()) {
        memset(buff, '\x00', BUFF_LEN);

        ether_server->setNextProt(htons(ETH_P_IP));
        int res = ether_server->recv_next_msg(buff, BUFF_LEN);

        struct iphdr *iph = (struct iphdr *)buff;

        char *packet_data = buff + sizeof(struct iphdr);
        int packet_data_sz = res - sizeof(struct iphdr);

        uint8_t* alloc_msg = new uint8_t[packet_data_sz];
        memcpy(alloc_msg, packet_data, packet_data_sz);

        protocolQueue.prots[iph->protocol].push(
                {std::unique_ptr<uint8_t>(alloc_msg), packet_data_sz}
        );

        last_client.raw = ntohl(iph->saddr);

    }

    message next_msg = std::move(
            protocolQueue.prots[getNextProt()].front()
    );
    protocolQueue.prots[getNextProt()].pop();

    int copy_cnt = std::min(count, next_msg.sz);
    memcpy(data, next_msg.data.get(), copy_cnt);
    return copy_cnt;

}

int ip4_conn_server::send_next_msg(ip4_addr client, void *buff, int count) {
//    struct sockaddr_in sock;
//    memset(&sock, 0, sizeof(sock));
//    sock.sin_addr.s_addr = htonl(client.raw);
//
//    // just before sending, make sure there is a file descriptor for this protocol
//    if (!prot_handlers.count(getNextProt())) register_filter(getNextProt());
//
//    return sendto(prot_handlers[getNextProt()].addit_data.fd,
//           buff, count,
//           0,
//           reinterpret_cast<const sockaddr *>(&sock), sizeof(sock));

#define BUFF_LEN 512
    char packet[BUFF_LEN];
    memset(packet, '\x00', BUFF_LEN);

    struct iphdr *iph = (struct iphdr *)packet;

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    int ip_packet_len = sizeof(struct iphdr) + count;
    iph->tot_len = htons(ip_packet_len);
    iph->id = htonl(4444);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = getNextProt();
    iph->check = 0;

    iph->saddr = inet_addr("10.100.102.18");
    iph->daddr = htonl(client.raw);

    iph->check = checksum((unsigned short *)packet, ip_packet_len >> 1);

    char *data = packet + sizeof(struct iphdr);
    memcpy(data, buff, count);

    ether_server->change_dest_mac(
            {0xc4,0xeb,0x42,0xed,0xc5,0xb7} // gateway
    );
    ether_server->setNextProt(htons(ETH_P_IP));
    return ether_server->send_next_msg(packet, ip_packet_len);
}

