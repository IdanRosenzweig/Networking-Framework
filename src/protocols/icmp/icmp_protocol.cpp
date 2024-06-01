#include "icmp_protocol.h"

#include "../protocol_t.h"
#include "../ip4/internet_checksum.h"

#include <cstring>
using namespace std;

icmp_protocol::icmp_protocol() {
    // prevent linux from sending various response icmp packets that interfere
    system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_all > /dev/null");
    system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts > /dev/null");
}

int icmp_protocol::send_data(send_msg_t &&data) {
    uint8_t *buff = data.get_reserve_buff();

    // header
    struct icmp_header header;
    header.type = next_type.get_next_choice();
    header.code = next_code.get_next_choice();
    header.content.raw = next_content.get_next_choice();
    header.checksum = 0;

    write_in_network_order(buff, &header);

    // plain data
    memcpy(buff + sizeof(icmp_header), data.get_active_buff(), data.get_count());

    // 0 is now substituted for the checksum. calculate the checksum value over the header and the content
    ((struct icmp_header*) buff)->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + data.get_count());

    data.toggle_active_buff();
    data.set_count(sizeof(icmp_header) + data.get_count());
    return send_medium.send_data(std::move(data));
}

void icmp_protocol::handle_callback(recv_msg_t &&data) {
    uint8_t *buff = data.buff_at_curr_offset();

    struct icmp_header header;
    extract_from_network_order(&header, buff);

    data.protocol_offsets.push_back({data.curr_offset, ICMP});
    data.curr_offset += sizeof(struct icmp_header);

    if (default_listener != nullptr) {
        recv_msg_t copy(data);
        default_listener->handle_callback(std::move(copy));
    }

    for (auto listener: listeners.stream_array({header.type, header.code})) {
        recv_msg_t copy(data);
        listener->handle_callback(std::move(copy));
    }

}

