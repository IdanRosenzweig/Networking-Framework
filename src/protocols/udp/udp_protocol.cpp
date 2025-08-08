#include "udp_protocol.h"

#include "src/protocols/ip4/internet_checksum.h"

#include <cstring>
using namespace std;

// udp_protocol::udp_protocol() {
//     system("sudo iptables -A OUTPUT -p icmp --icmp-type destination-unreachable -j DROP > /dev/null");
// }

// int udp_protocol::send_data(send_msg_t &&data) {
//     uint8_t *buff = data.get_reserve_buff();

//     // udp header
//     struct udp_header header;
//     header.source_port = next_source_port.get_next_choice();
//     header.dest_port = next_dest_port.get_next_choice();
//     header.len = sizeof(udp_header) + data.get_count();
//     header.checksum = 0;

//     write_in_network_order(buff, &header);

//     // plain_data
//     uint8_t *packet_data = buff + sizeof(udp_header);
//     memcpy(packet_data, data.get_active_buff(), data.get_count());

//     data.toggle_active_buff();
//     data.set_count(sizeof(udp_header) + data.get_count());
//     return send_medium.send_data(std::move(data));
// }

// void udp_protocol::handle_callback(recv_msg_t &&data) {
//     uint8_t *buff = data.buff_at_curr_offset();

//     struct udp_header header;
//     extract_from_network_order(&header, buff);

//     data.protocol_offsets.push_back({data.curr_offset, UDP});
//     data.curr_offset += sizeof(struct udp_header);


//     if (default_listener != nullptr) {
//         recv_msg_t copy(data);
//         default_listener->handle_callback(std::move(copy));
//     }

//     for (auto listener: listeners.stream_array({header.dest_port})) {
//         recv_msg_t copy(data);
//         listener->handle_callback(std::move(copy));
//     }

// }




void udp_protocol::send(shared_ptr<net_access> const& udp_surface, udp_header const& header, vector<uint8_t> const& data) {
    if (udp_surface == nullptr) return;

    vector<uint8_t> buff(sizeof(udp_header) + data.size());

    // udp header
    udp_header tmp = header;
    tmp.len = sizeof(udp_header) + data.size();
    tmp.checksum = 0;
    write_in_network_order(buff.data(), &tmp);

    // encapsulated data
    memcpy(buff.data() + sizeof(struct udp_header), data.data(), data.size());

    // checksum
    // tmp.checksum = internet_checksum(buff.data(), sizeof(udp_header));
    // write_in_network_order(buff.data(), &tmp); // todo something smarter here

    udp_surface->get_send_access()->send_data(buff);
}

void udp_protocol::connect_recv(
    shared_ptr<net_access> const& udp_surface, shared_ptr<recv_listener<pair<udp_header, vector<uint8_t>>>> const& recv,
    optional<udp_port> src_port, optional<udp_port> dest_port
) {
    if (udp_surface == nullptr) return;

    struct my_recv : public recv_listener_bytes {
        shared_ptr<recv_listener<pair<udp_header, vector<uint8_t>>>> recv;
        optional<udp_port> src_port;
        optional<udp_port> dest_port;
        my_recv(shared_ptr<recv_listener<pair<udp_header, vector<uint8_t>>>> const& recv, optional<udp_port> src_port, optional<udp_port> dest_port) : recv(recv), src_port(src_port), dest_port(dest_port) {}

        void handle_recv(vector<uint8_t> const& data) override {
            udp_header header;
            extract_from_network_order(&header, data.data());

            vector<uint8_t> encap_data(data.data() + sizeof(udp_header), data.data() + data.size());

            // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
            if (src_port.has_value()) {
                if (header.src_port != src_port.value()) {
                    // cout << "src port no match, needed " << src_port.value() << " got " << header.src_port << endl;
                    return;
                }
            }

            if (dest_port.has_value()) {
                if (header.dest_port != dest_port.value()) {
                    // cout << "dest port no match, needed " << dest_port.value() << " got " << header.dest_port << endl;
                    return;
                }
            }

            recv->handle_recv({header, encap_data});
        }
    };

    udp_surface->set_recv_access(make_shared<my_recv>(recv, src_port, dest_port));
}


void udp_protocol::connect_net_access_generator_listener(shared_ptr<net_access> const& access, udp_port dest_port, shared_ptr<net_access_generator_listener>&& listener) {
    struct my_recv : public recv_listener<pair<udp_header, vector<uint8_t>>> {
        udp_port dest_port;
        multi_net_access multi_access;
        shared_ptr<net_access_generator_listener> listener;

        my_recv(udp_port dest_port, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_port(dest_port), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
        }

        void handle_recv(pair<udp_header, vector<uint8_t>> const& data) {
            // cout << "udp generator called" << endl;
            if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

            // create a new net access
            auto new_net_access = make_shared<udp_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_port, dest_port);

            // forward it to the generator
            listener->handle_new_net_access(new_net_access);
            
            // send the data
            // cout << "udp generator handles data" << endl;
            if (new_net_access->recv_access != nullptr) {
                // cout << "udp generator sends data" << endl;
                new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
            }
        }
    };

    udp_protocol::connect_recv(access, make_shared<my_recv>(dest_port, access, std::move(listener)), {}, dest_port);
}