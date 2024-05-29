#include "icmp_protocol.h"
#include <iostream>
#include <cstring>
#include "icmp_header.h"
#include "../ip4/internet_checksum.h"

using namespace std;

int icmp_protocol::send_data(send_msg<> &&msg) {
    uint8_t *buff = msg.get_reserve_buff();

    struct icmp_header *packet = reinterpret_cast<icmp_header *>(buff);
    packet->type = next_type.get_next_choice();
    packet->code = next_code.get_next_choice();
    packet->content.raw = next_content.get_next_choice();

    memcpy(buff + sizeof(icmp_header), msg.get_active_buff(), msg.get_count());

    packet->checksum = 0;
    packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(sizeof(icmp_header) + msg.get_count());
    return send_medium.send_data(std::move(msg));
}

void icmp_protocol::handle_received_event(received_msg &&msg) {
    uint8_t *buff = msg.data.data() + msg.curr_offset;

    struct icmp_header *icmp = (struct icmp_header *) buff;

    uint8_t type = icmp->type;
    uint8_t code = icmp->code;

    msg.protocol_offsets.push_back({msg.curr_offset, ICMP});
    msg.curr_offset += sizeof(struct icmp_header);

    if (default_listener != nullptr) {
        received_msg copy(msg);
        default_listener->handle_received_event(std::move(copy));
    }

    for (auto listener: listeners.stream_array({type, code})) {
        received_msg copy(msg);
        listener->handle_received_event(std::move(copy));
    }

}

