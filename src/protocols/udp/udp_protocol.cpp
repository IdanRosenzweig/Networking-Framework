#include "udp_protocol.h"

#include "../ip4/internet_checksum.h"
#include "../protocol_t.h"

#include <cstring>
using namespace std;

udp_protocol::udp_protocol() {
    system("sudo iptables -A OUTPUT -p icmp --icmp-type destination-unreachable -j DROP > /dev/null");
}

int udp_protocol::send_data(send_msg_t &&data) {
    uint8_t *buff = data.get_reserve_buff();

    // udp header
    struct udp_header header;
    header.source_port = next_source_port.get_next_choice();
    header.dest_port = next_dest_port.get_next_choice();
    header.len = sizeof(udp_header) + data.get_count();
    header.checksum = 0;

    write_in_network_order(buff, &header);

    // plain_data
    uint8_t *packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data.get_active_buff(), data.get_count());

    data.toggle_active_buff();
    data.set_count(sizeof(udp_header) + data.get_count());
    return send_medium.send_data(std::move(data));
}

void udp_protocol::handle_callback(recv_msg_t &&data) {
    uint8_t *buff = data.buff_at_curr_offset();

    struct udp_header header;
    extract_from_network_order(&header, buff);

    data.protocol_offsets.push_back({data.curr_offset, UDP});
    data.curr_offset += sizeof(struct udp_header);


    if (default_listener != nullptr) {
        recv_msg_t copy(data);
        default_listener->handle_callback(std::move(copy));
    }

    for (auto listener: listeners.stream_array({header.dest_port})) {
        recv_msg_t copy(data);
        listener->handle_callback(std::move(copy));
    }

}

