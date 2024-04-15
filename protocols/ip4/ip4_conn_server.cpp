#include "ip4_conn_server.h"
#include <iostream>
#include <netinet/ip.h>
#include <cstring>
#include <arpa/inet.h>
#include "checksum.h"

using namespace std;

int ip4_conn_server::send_data(void *buff, int count) {
    if (next_client_addr.get_next_choice().raw == 0) {
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
    iph->protocol = next_external_protocol.get_next_choice();
    iph->check = 0;

    iph->saddr = inet_addr("10.100.102.18");
    iph->daddr = htonl(next_client_addr.get_next_choice().raw);

    iph->check = checksum((unsigned short *) packet, ip_packet_len >> 1);

    char *data = packet + sizeof(struct iphdr);
    memcpy(data, buff, count);

//    ether_server->setNextClient(
//            {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
//    );
//    ether_server->setNextProt(htons(ETH_P_IP));
//    return ether_server->send_next_msg(packet, ip_packet_len);
    return gateway->send_data(packet, ip_packet_len);
}

void ip4_conn_server::handle_received_msg(msg_protocol_stack &&msg) {
//    cout << "ip server handler called" << endl;
    uint8_t *buff = msg.msg.data.get() + msg.curr_offset;

    struct iphdr *iph = (struct iphdr *) buff;
    uint8_t protocol = iph->protocol;

    msg.protocol_offsets.push(msg.curr_offset);

    if (prot_handlers.count(protocol)) {
        msg.curr_offset += sizeof(struct iphdr);
        prot_handlers[protocol]->handle_received_msg(std::move(msg));
    }

}
