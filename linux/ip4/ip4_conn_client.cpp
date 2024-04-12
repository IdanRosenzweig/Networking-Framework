#include "ip4_conn_client.h"
#include <iostream>
#include <netinet/ip.h>
#include <cstring>
#include <linux/if_ether.h>
#include "checksum.h"

using namespace std;

ip4_conn_client::ip4_conn_client(const string &ip) : ip(ip) {
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());

}

int ip4_conn_client::recv_next_msg( void* data, int count) {
    int prot = getNextProt();

#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    if (prot == IPPROTO_ICMP)
        cout << "waiting until icmp buffer has message" << endl;
    while (protocolQueue.prots[prot].empty()) {
        memset(buff, '\x00', BUFF_LEN);

        ether_client->setNextProt(htons(ETH_P_IP));
        int res = ether_client->recv_next_msg(buff, BUFF_LEN);

        struct iphdr *iph = (struct iphdr *)buff;

        char *packet_data = buff + sizeof(struct iphdr);
        int packet_data_sz = res - sizeof(struct iphdr);

        uint8_t* alloc_msg = new uint8_t[packet_data_sz];
        memcpy(alloc_msg, packet_data, packet_data_sz);

        if (iph->protocol == IPPROTO_ICMP)
            cout << "received icmp back, whole size " << res << endl;

        protocolQueue.prots[iph->protocol].push_back(
                {std::unique_ptr<uint8_t>(alloc_msg), packet_data_sz}
        );
    }

    if (prot == IPPROTO_ICMP)
        cout << "icmp buff not empty, poping" << endl;
    message next_msg = std::move(
            protocolQueue.prots[prot].front()
    );
    protocolQueue.prots[prot].pop_front();

    int copy_cnt = std::min(count, next_msg.sz);
    if (prot == IPPROTO_ICMP) cout << "icmp sz " << copy_cnt << endl;
    memcpy(data, next_msg.data.get(), copy_cnt);
    return copy_cnt;

}

int ip4_conn_client::send_next_msg( void *buff, int count) {
#define BUFF_LEN 1024
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
    iph->daddr = dest_addr.sin_addr.s_addr;

    iph->check = checksum((unsigned short *)packet, ip_packet_len >> 1);

    char *data = packet + sizeof(struct iphdr);
    memcpy(data, buff, count);

    ether_client->change_dest_mac(
            {0xc4,0xeb,0x42,0xed,0xc5,0xb7} // gateway
            );
    ether_client->setNextProt(htons(ETH_P_IP));
    return ether_client->send_next_msg(packet, ip_packet_len);
}