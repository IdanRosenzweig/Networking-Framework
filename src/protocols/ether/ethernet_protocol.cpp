#include "ethernet_protocol.h"

#include "ethernet_header.h"
#include "../protocol_t.h"

#include <cstring>
using namespace std;

ethernet_protocol::ethernet_protocol() {
    next_dest_addr.set_next_choice(BROADCAST_MAC);
}

int ethernet_protocol::send_data(send_msg_t&&data) {
    uint8_t* buff = data.get_reserve_buff();

    // ethernet header
    struct ethernet_header header;
    header.dest_addr = next_dest_addr.get_next_choice();
    header.source_addr = next_source_addr.get_next_choice();
    header.ether_type = next_protocol.get_next_choice();

    write_in_network_order(buff, &header);

    // plain buff
    uint8_t *frame_data = buff + sizeof(struct ethernet_header);
    memcpy(frame_data, data.get_active_buff(), data.get_count());

    data.toggle_active_buff();
    data.set_count(sizeof(struct ethernet_header) + data.get_count());
    return send_medium.send_data(std::move(data));

}

void ethernet_protocol::handle_callback(recv_msg_t&& data) {
    uint8_t *buff = data.buff_at_curr_offset();

    struct ethernet_header eth_header;
    extract_from_network_order(&eth_header, buff);

    data.protocol_offsets.push_back({data.curr_offset, ETHERNET});
    data.curr_offset += sizeof(struct ethernet_header);

    if (default_listener != nullptr) {
        recv_msg_t copy(data);
        default_listener->handle_callback(std::move(copy));
    }

    for (auto listener : listeners.stream_array({eth_header.ether_type, eth_header.dest_addr, eth_header.source_addr})) {
        recv_msg_t copy(data);
        listener->handle_callback(std::move(copy));
    }

}
