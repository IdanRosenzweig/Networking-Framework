#include "ip4_protocol.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "internet_checksum.h"
#include <netinet/ip.h>

using namespace std;

ip4_protocol::ip4_protocol() {
    next_ttl.set_next_choice(255);
}


int ip4_protocol::send_data(send_msg<>&& msg) {
    if (next_dest_addr.get_next_choice() == empty_ip4_addr) {
        std::cout << "ip dest is null" << endl;
        return 0;
    }

    uint8_t* packet = msg.get_reserve_buff();

    struct iphdr *iph = (struct iphdr *) packet;

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    int ip_packet_len = sizeof(struct iphdr) + msg.get_count();
    iph->tot_len = htons(ip_packet_len);
    iph->id = htons(4444);
    iph->frag_off = 0;
    iph->ttl = next_ttl.get_next_choice();
    iph->protocol = next_protocol.get_next_choice();

//    iph->saddr = htonl(next_source_addr.get_next_choice().raw);
//    iph->daddr = htonl(next_dest_addr.get_next_choice().raw);
    write_in_network_order((uint8_t*) &iph->saddr, &next_source_addr.get_next_choice());
    write_in_network_order((uint8_t*) &iph->daddr, &next_dest_addr.get_next_choice());

    iph->check = 0;
//    iph->check = internet_checksum((uint16_t *) packet, ip_packet_len);
    iph->check = internet_checksum((uint16_t *) packet, sizeof(struct iphdr));

    uint8_t *data = packet + sizeof(struct iphdr);
    memcpy(data, msg.get_active_buff(), msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(ip_packet_len);
    return gateway->send_data(std::move(msg));
}

void ip4_protocol::handle_received_event(received_msg&& msg) {
    uint8_t *buff = msg.data.data() + msg.curr_offset;

    struct iphdr *iph = (struct iphdr *) buff;
    uint8_t protocol = iph->protocol;

    msg.protocol_offsets.push_back({msg.curr_offset, IP4});
    msg.curr_offset += sizeof(struct iphdr);


    if (default_handler != nullptr) {
        received_msg copy(msg);
        default_handler->handle_received_event(std::move(copy));
    }

    if (protocol_handlers.count(protocol)) {
        for (auto& handler : protocol_handlers[protocol]) {
            received_msg copy(msg);
            handler->handle_received_event(std::move(copy));
        }
    }

}
