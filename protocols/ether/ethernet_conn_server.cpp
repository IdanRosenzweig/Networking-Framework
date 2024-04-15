#include "ethernet_conn_server.h"

#include <iostream>
#include <cstring>
#include <netinet/if_ether.h>

using namespace std;

ethernet_conn_server::ethernet_conn_server() {
    next_dest_addr.set_next_choice(BROADCAST_MAC);
}


int ethernet_conn_server::send_data(void *data, int count) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, 0, BUFF_LEN);

    // setup ethernet
    struct ether_header *eth_header = (struct ether_header *) buff;

    // dest mac addr

    mac_addr dest_device = next_dest_addr.get_next_choice();
    memcpy(eth_header->ether_dhost, &dest_device, ETH_ALEN);
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast

// source (my mac)
    mac_addr src_addr = my_addr.get_next_choice();
    memcpy(eth_header->ether_shost, &src_addr, ETH_ALEN); // my mac

    // ethertype
    eth_header->ether_type = next_protocol.get_next_choice();


    char *frame_data = buff + sizeof(struct ether_header);
    memcpy(frame_data, data, count);

    return gateway->send_data(buff, sizeof(struct ether_header) + count);

}

void ethernet_conn_server::handle_received_msg(msg_protocol_stack&& msg) {
//    cout << "ethernet server handler called" << endl;
    uint8_t *buff = msg.msg.data.get() + msg.curr_offset;

    struct ether_header *eth_header = (struct ether_header *) buff;
    uint16_t ethertype = eth_header->ether_type;

    msg.protocol_offsets.push(msg.curr_offset);

    if (prot_handlers.count(ethertype)) {
        msg.curr_offset += sizeof(struct ether_header);
        prot_handlers[ethertype]->handle_received_msg(std::move(msg));
    }

}
