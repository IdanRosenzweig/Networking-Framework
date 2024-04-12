#include "ip4_conn_server.h"
#include <iostream>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include "checksum.h"

using namespace std;

ip4_conn_server::ip4_conn_server() {
    cout << "waiting" << endl;
}


int ip4_conn_server::recv_next_msg(void *data, int count) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    while (protocolQueue.prots[getNextProt()].empty()) {
        memset(buff, '\x00', BUFF_LEN);

        ether_server->setNextProt(htons(ETH_P_IP));
        int res = ether_server->recv_next_msg(buff, BUFF_LEN);

        struct iphdr *iph = (struct iphdr *) buff;

        char *packet_data = buff + sizeof(struct iphdr);
        int packet_data_sz = res - sizeof(struct iphdr);

        uint8_t *alloc_msg = new uint8_t[packet_data_sz];
        memcpy(alloc_msg, packet_data, packet_data_sz);

        protocolQueue.prots[iph->protocol].push_back(
                {std::unique_ptr<uint8_t>(alloc_msg), packet_data_sz}
        );

    }

    message next_msg = std::move(
            protocolQueue.prots[getNextProt()].front()
    );
    protocolQueue.prots[getNextProt()].pop_front();

    int copy_cnt = std::min(count, next_msg.sz);
    memcpy(data, next_msg.data.get(), copy_cnt);
    return copy_cnt;

}

int ip4_conn_server::send_next_msg(void *buff, int count) {
    if (getNextClient().raw == 0) {
        cout << "client is null" << endl;
        return 0;
    }

#define BUFF_LEN 1024
    char packet[BUFF_LEN];
    memset(packet, '\x00', BUFF_LEN);

    struct iphdr *iph = (struct iphdr *) packet;

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
    iph->daddr = htonl(getNextClient().raw);

    iph->check = checksum((unsigned short *) packet, ip_packet_len >> 1);

    char *data = packet + sizeof(struct iphdr);
    memcpy(data, buff, count);

    ether_server->setNextClient(
            {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
    );
    ether_server->setNextProt(htons(ETH_P_IP));
    return ether_server->send_next_msg(packet, ip_packet_len);
}

