#include "ethernet_protocol.h"

#include <iostream>
#include <cstring>
#include <netinet/if_ether.h>

using namespace std;

ethernet_protocol::ethernet_protocol() {
    next_dest_addr.set_next_choice(BROADCAST_MAC);
}


int ethernet_protocol::send_data(send_msg msg) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, 0, BUFF_LEN);

    // setup ethernet
    struct ether_header *eth_header = (struct ether_header *) buff;

    // dest mac octets

    mac_addr dest_device = next_dest_addr.get_next_choice();
    memcpy(eth_header->ether_dhost, &dest_device, ETH_ALEN);
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast

// source (my mac)
    mac_addr src_addr = next_source_addr.get_next_choice();
    memcpy(eth_header->ether_shost, &src_addr, ETH_ALEN); // my mac

    // ethertype
    eth_header->ether_type = next_protocol.get_next_choice();


    char *frame_data = buff + sizeof(struct ether_header);
    memcpy(frame_data, msg.buff, msg.count);

    return gateway->send_data({buff, (int) sizeof(struct ether_header) + msg.count});

}

void ethernet_protocol::handle_received_event(received_msg msg) {
//    cout << "ethernet server handler called" << endl;
    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct ether_header *eth_header = (struct ether_header *) buff;
    uint16_t ethertype = eth_header->ether_type;

    msg.protocol_offsets.push_back({msg.curr_offset, ETHERNET});

    if (protocol_handlers.is_key_assigned(ethertype)) {
        msg.curr_offset += sizeof(struct ether_header);
        protocol_handlers.get_val_of_key(ethertype)->handle_received_event(std::move(msg));
    }

}
