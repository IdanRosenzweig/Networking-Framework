#include "ip4_protocol.h"

#include "internet_checksum.h"

#include <cstring>
using namespace std;

namespace ip4_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ip4_header const& header, vector<uint8_t> const& data) {
        if (net_access == nullptr) return;

        vector<uint8_t> buff(sizeof(ip4_header) + data.size());

        // ip4 header
        ip4_header tmp_header = header;
        tmp_header.checksum = 0;
        tmp_header.checksum = internet_checksum((uint16_t *)&tmp_header, sizeof(struct ip4_header));
        write_in_network_order(buff.data(), &tmp_header);

        // encapsulated data
        memcpy(buff.data() + sizeof(struct ip4_header), data.data(), data.size());

        net_access->get_send_access()->send_data(buff);
    }

    void send(ip4_routing_table const& routing_table, ip4_header const& header, vector<uint8_t> const& data) {
        ip4_protocol::send(routing_table.find_route(header.dest_addr), header, data);
    }

    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<basic_recv_listener<pair<ip4_header, vector<uint8_t>>>> const& recv,
        optional<ip4_addr> src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot
    ) {
        if (net_access == nullptr) return;

        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
            shared_ptr<basic_recv_listener<pair<ip4_header, vector<uint8_t>>>> recv;
            optional<ip4_addr> src_addr;
            optional<ip4_addr> dest_addr;
            optional<ip_prot_values> prot;
            my_recv(shared_ptr<basic_recv_listener<pair<ip4_header, vector<uint8_t>>>> const& recv, optional<ip4_addr> src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : recv(recv), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

            void handle_recv(vector<uint8_t> const& data) override {
                ip4_header ip_header;
                extract_from_network_order(&ip_header, data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ip4_header), data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_addr.has_value()) {
                    if (ip_header.src_addr != src_addr.value()) {return;}
                }

                if (dest_addr.has_value()) {
                    if (ip_header.dest_addr != dest_addr.value()) return;
                }

                if (prot.has_value()) {
                    if (ip_header.prot != prot.value()) return;
                }

                recv->handle_recv({ip_header, encap_data});
            }
        };

        net_access->set_recv_access(make_shared<my_recv>(recv, src_addr, dest_addr, prot));
    }

}






// ip4_protocol::ip4_protocol() {
//     next_ttl.set_next_choice(255);
// }

// int ip4_protocol::send_data(send_msg_t &&data) {
//     uint8_t *buff = data.get_reserve_buff();

//     // ip4 header
//     struct ip4_header header;
//     header.header_len = 5; // no additional options
//     header.version = 4;
//     header.type_of_service = 0;
//     int ip_packet_len = sizeof(struct ip4_header) + data.get_count();
//     header.total_len = ip_packet_len;
//     header.id = 6969;
//     header.flags = 0;
//     header.frag_off = 0;
//     header.ttl = next_ttl.get_next_choice();
//     header.protocol = next_protocol.get_next_choice();
//     header.checksum = 0;
//     header.source_addr = next_source_addr.get_next_choice();
//     header.dest_addr = next_dest_addr.get_next_choice();

//     write_in_network_order(buff, &header);

//     // 0 is now substituted for the checksum. calculate the checksum value over the header
//     ((struct ip4_header *) buff)->checksum = internet_checksum((uint16_t *) buff, sizeof(struct ip4_header));

//     // plain buff
//     uint8_t *plain_data = buff + sizeof(struct ip4_header);
//     memcpy(plain_data, data.get_active_buff(), data.get_count());

//     data.toggle_active_buff();
//     data.set_count(ip_packet_len);
//     return send_medium.send_data(std::move(data));
// }

// void ip4_protocol::handle_callback(recv_msg_t &&data) {
//     uint8_t *buff = data.buff_at_curr_offset();

//     struct ip4_header header;
//     extract_from_network_order(&header, buff);

//     data.protocol_offsets.push_back({data.curr_offset, IP4});
//     data.curr_offset += sizeof(struct ip4_header);


//     if (default_listener != nullptr) {
//         recv_msg_t copy(data);
//         default_listener->handle_callback(std::move(copy));
//     }

//     for (auto listener: listeners.stream_array({header.protocol, header.source_addr, header.dest_addr})) {
//         recv_msg_t copy(data);
//         listener->handle_callback(std::move(copy));
//     }

// }
