#include "ip4_protocol.h"

#include "../protocol_t.h"
#include "ip4_header.h"
#include "internet_checksum.h"

#include <cstring>

using namespace std;

ip4_protocol::ip4_protocol() {
    next_ttl.set_next_choice(255);
}

int ip4_protocol::send_data(send_msg_t &&data) {
    uint8_t *buff = data.get_reserve_buff();

    // ip4 header
    struct ip4_header header;
    header.header_len = 5; // no additional options
    header.version = 4;
    header.type_of_service = 0;
    int ip_packet_len = sizeof(struct ip4_header) + data.get_count();
    header.total_len = ip_packet_len;
    header.id = 6969;
    header.flags = 0;
    header.frag_off = 0;
    header.ttl = next_ttl.get_next_choice();
    header.protocol = next_protocol.get_next_choice();
    header.checksum = 0;
    header.source_addr = next_source_addr.get_next_choice();
    header.dest_addr = next_dest_addr.get_next_choice();

    write_in_network_order(buff, &header);

    // 0 is now substituted for the checksum. calculate the checksum value over the header
    ((struct ip4_header *) buff)->checksum = internet_checksum((uint16_t *) buff, sizeof(struct ip4_header));

    // plain buff
    uint8_t *plain_data = buff + sizeof(struct ip4_header);
    memcpy(plain_data, data.get_active_buff(), data.get_count());

    data.toggle_active_buff();
    data.set_count(ip_packet_len);
    return send_medium.send_data(std::move(data));
}

void ip4_protocol::handle_callback(recv_msg_t &&data) {
    uint8_t *buff = data.buff_at_curr_offset();

    struct ip4_header header;
    extract_from_network_order(&header, buff);

    data.protocol_offsets.push_back({data.curr_offset, IP4});
    data.curr_offset += sizeof(struct ip4_header);


    if (default_listener != nullptr) {
        recv_msg_t copy(data);
        default_listener->handle_callback(std::move(copy));
    }

    for (auto listener: listeners.stream_array({header.protocol, header.source_addr, header.dest_addr})) {
        recv_msg_t copy(data);
        listener->handle_callback(std::move(copy));
    }

}
