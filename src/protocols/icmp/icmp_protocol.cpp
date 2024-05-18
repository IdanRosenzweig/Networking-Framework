#include "icmp_protocol.h"
#include <iostream>
#include <linux/icmp.h>
#include <cstring>
#include <thread>
#include "icmp_header.h"
#include "../ip4/internet_checksum.h"

using namespace std;

int icmp_protocol::send_data(send_msg<>& msg) {
    uint8_t* buff = msg.get_reserve_buff();

    icmp_header *packet = reinterpret_cast<icmp_header*>(buff);
    packet->type = next_type.get_next_choice();
    packet->code = next_code.get_next_choice();
    packet->content.raw = next_content.get_next_choice();

    memcpy(buff + sizeof(icmp_header), msg.get_active_buff(), msg.get_count());

    packet->checksum = 0;
    packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(sizeof(icmp_header) + msg.get_count());
    return gateway->send_data(msg);
}

void icmp_protocol::handle_received_event(received_msg& msg) {
//    raw_message_q.push_back(msg);

    uint8_t *buff = msg.data.data() + msg.curr_offset;

    struct icmp_header *icmp = (struct icmp_header *) buff;
    uint8_t type = icmp->type;

    msg.protocol_offsets.push_back({msg.curr_offset, ICMP});
    msg.curr_offset += sizeof(struct icmp_header);

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

    if (type_handlers.is_key_assigned(type)) {
        type_handlers.get_val_of_key(type)->handle_received_event(msg);
//        return;
    }

}

