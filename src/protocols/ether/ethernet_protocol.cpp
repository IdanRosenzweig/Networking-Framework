#include "ethernet_protocol.h"

#include <iostream>
#include <cstring>

using namespace std;

ethernet_protocol::ethernet_protocol() {
    next_dest_addr.set_next_choice(BROADCAST_MAC);
}

int ethernet_protocol::send_data(send_msg<>&& msg) {
    uint8_t* buff = msg.get_reserve_buff();

    struct ether_header *eth_header = (struct ether_header *) buff;

    // dest mac addr
    mac_addr dest_device = next_dest_addr.get_next_choice();
    write_in_network_order(eth_header->ether_dhost, &dest_device);

    // source mac addr
    mac_addr src_addr = next_source_addr.get_next_choice();
    write_in_network_order(eth_header->ether_shost, &src_addr); // my mac

    // ethertype
    eth_header->ether_type = next_protocol.get_next_choice();

    // plain data
    uint8_t *frame_data = buff + sizeof(struct ether_header);
    memcpy(frame_data, msg.get_active_buff(), msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(sizeof(struct ether_header) + msg.get_count());
    return send_medium.send_data(std::move(msg));

}

void ethernet_protocol::handle_received_event(received_msg&& msg) {
//    cout << "ethernet protocol received. forwarding" << endl;
    uint8_t *buff = msg.data.data() + msg.curr_offset;

    struct ether_header *eth_header = (struct ether_header *) buff;
    uint16_t ethertype = eth_header->ether_type;
    mac_addr src_mac;
    extract_from_network_order(&src_mac, eth_header->ether_shost);
    mac_addr dest_mac;
    extract_from_network_order(&dest_mac, eth_header->ether_dhost);

    msg.protocol_offsets.push_back({msg.curr_offset, ETHERNET});
    msg.curr_offset += sizeof(struct ether_header);

    if (default_listener != nullptr) {
        received_msg copy(msg);
        default_listener->handle_received_event(std::move(copy));
    }

    for (auto listener : listeners.stream_array({ethertype, src_mac, dest_mac})) {
        received_msg copy(msg);
        listener->handle_received_event(std::move(copy));
    }

}
