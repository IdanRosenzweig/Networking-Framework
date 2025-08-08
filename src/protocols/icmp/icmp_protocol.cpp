#include "icmp_protocol.h"

#include "../ip4/internet_checksum.h"

#include <cstring>
using namespace std;

void icmp_protocol::send(shared_ptr<net_access> const& icmp_surface, icmp_header const& header, vector<uint8_t> const& data) {
    if (icmp_surface == nullptr) return;

    vector<uint8_t> buff(sizeof(icmp_header) + data.size());

    // arp header
    icmp_header tmp_header = header;
    tmp_header.checksum = 0;
    write_in_network_order(buff.data(), &tmp_header);

    // encapsulated data
    memcpy(buff.data() + sizeof(struct icmp_header), data.data(), data.size());

    // update checksum
    tmp_header.checksum = internet_checksum(buff.data(), buff.size());
    write_in_network_order(buff.data(), &tmp_header); // todo something smarter here

    icmp_surface->get_send_access()->send_data(buff);
}

void icmp_protocol::connect_recv(
    shared_ptr<net_access> const& icmp_surface, shared_ptr<recv_listener<pair<icmp_header, vector<uint8_t>>>> const& recv,
    optional<icmp_type> type
) {
    if (icmp_surface == nullptr) return;

    struct my_recv : public recv_listener_bytes {
        shared_ptr<recv_listener<pair<icmp_header, vector<uint8_t>>>> recv;
        optional<icmp_type> type;
        my_recv(shared_ptr<recv_listener<pair<icmp_header, vector<uint8_t>>>> const& recv, optional<icmp_type> type) : recv(recv), type(type) {}

        void handle_recv(vector<uint8_t> const& data) override {
            struct icmp_header header;
            extract_from_network_order(&header, data.data());

            if (type.has_value()) {
                if (header.type != type.value()) return;
            }

            recv->handle_recv({header, data});
        }
    };

    icmp_surface->set_recv_access(make_shared<my_recv>(recv, type));
}

// icmp_protocol::icmp_protocol() {
//     // prevent linux from sending various response icmp packets that interfere
//     system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_all > /dev/null");
//     system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts > /dev/null");
// }

// int icmp_protocol::send_data(send_msg_t &&data) {
//     uint8_t *buff = data.get_reserve_buff();

//     // header
//     struct icmp_header header;
//     header.type = next_type.get_next_choice();
//     header.code = next_code.get_next_choice();
//     header.content.raw = next_content.get_next_choice();
//     header.checksum = 0;

//     write_in_network_order(buff, &header);

//     // plain data
//     memcpy(buff + sizeof(icmp_header), data.get_active_buff(), data.get_count());

//     // 0 is now substituted for the checksum. calculate the checksum value over the header and the content
//     ((struct icmp_header*) buff)->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + data.get_count());

//     data.toggle_active_buff();
//     data.set_count(sizeof(icmp_header) + data.get_count());
//     return send_medium.send_data(std::move(data));
// }

// void icmp_protocol::handle_callback(recv_msg_t &&data) {
//     uint8_t *buff = data.buff_at_curr_offset();

//     struct icmp_header header;
//     extract_from_network_order(&header, buff);

//     data.protocol_offsets.push_back({data.curr_offset, ICMP});
//     data.curr_offset += sizeof(struct icmp_header);

//     if (default_listener != nullptr) {
//         recv_msg_t copy(data);
//         default_listener->handle_callback(std::move(copy));
//     }

//     for (auto listener: listeners.stream_array({header.type, header.code})) {
//         recv_msg_t copy(data);
//         listener->handle_callback(std::move(copy));
//     }

// }

