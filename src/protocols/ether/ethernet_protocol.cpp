#include "ethernet_protocol.h"

#include <iostream>
#include <cstring>

using namespace std;

ethernet_protocol::ethernet_protocol() {
    next_dest_addr.set_next_choice(BROADCAST_MAC);
}


int ethernet_protocol::send_data(send_msg<>&& msg) {
    uint8_t* buff = msg.get_reserve_buff();

    // setup ethernet
    struct ether_header *eth_header = (struct ether_header *) buff;

    // dest mac octets

    mac_addr dest_device = next_dest_addr.get_next_choice();
    memcpy(eth_header->ether_dhost, &dest_device, ETH_ALEN);
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast

// source (my mac)
    mac_addr src_addr = next_source_addr.get_next_choice();
    memcpy(eth_header->ether_shost, &src_addr, ETH_ALEN); // my mac

    // ethertypeevent.data_t.get() + event.curr_offset
    eth_header->ether_type = next_protocol.get_next_choice();


    uint8_t *frame_data = buff + sizeof(struct ether_header);
    memcpy(frame_data, msg.get_active_buff(), msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(sizeof(struct ether_header) + msg.get_count());
    return gateway->send_data(std::move(msg));

}

void ethernet_protocol::handle_received_event(received_msg&& msg) {
    uint8_t *buff = msg.data.data() + msg.curr_offset;

    struct ether_header *eth_header = (struct ether_header *) buff;
    uint16_t ethertype = eth_header->ether_type;

    msg.protocol_offsets.push_back({msg.curr_offset, ETHERNET});
    msg.curr_offset += sizeof(struct ether_header);

    if (default_handler != nullptr) {
        received_msg copy(msg);
        default_handler->handle_received_event(std::move(copy));
    }

    if (protocol_handlers.is_key_assigned(ethertype)) {
        protocol_handlers.get_val_of_key(ethertype)->handle_received_event(std::move(msg));
//        return;
    }

}
