#include "ip4_protocol.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "internet_checksum.h"

using namespace std;

ip4_protocol::ip4_protocol() {
    next_ttl.set_next_choice(255);
}


int ip4_protocol::send_data(send_msg msg) {
    if (next_dest_addr.get_next_choice() == ip4_addr{0}) {
//        cout << "tcpSession is null" << endl;
        return 0;
    }

#define BUFF_LEN 1024
    char packet[BUFF_LEN];
    memset(packet, '\x00', BUFF_LEN);

    struct iphdr *iph = (struct iphdr *) packet;

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    int ip_packet_len = sizeof(struct iphdr) + msg.count;
    iph->tot_len = htons(ip_packet_len);
    iph->id = htonl(4444);
    iph->frag_off = 0;
    iph->ttl = next_ttl.get_next_choice();
    iph->protocol = next_protocol.get_next_choice();

//    iph->saddr = htonl(next_source_addr.get_next_choice().raw);
//    iph->daddr = htonl(next_dest_addr.get_next_choice().raw);
    write_in_network_order((uint8_t*) &iph->saddr, &next_source_addr.get_next_choice());
    write_in_network_order((uint8_t*) &iph->daddr, &next_dest_addr.get_next_choice());

    iph->check = 0;
    iph->check = internet_checksum((uint16_t *) packet, ip_packet_len);

    char *data = packet + sizeof(struct iphdr);
    memcpy(data, msg.buff, msg.count);

    return gateway->send_data({packet, ip_packet_len});
}

void ip4_protocol::handle_received_event(received_msg& msg) {
//    cout << "ip aggregator app_handler called" << endl;
    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct iphdr *iph = (struct iphdr *) buff;
    uint8_t protocol = iph->protocol;

    msg.protocol_offsets.push_back({msg.curr_offset, IP4});
    msg.curr_offset += sizeof(struct iphdr);

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

    if (protocol_handlers.is_key_assigned(protocol)) {
        protocol_handlers.get_val_of_key(protocol)->handle_received_event(msg);
    }

}

