#include "ethernet2_protocol.h"

#include <cstring>
using namespace std;

namespace ethernet2_protocol {
    void send(shared_ptr<net_access> const& ether2_surface, ethernet2_header const& header, vector<uint8_t> const& data) {
        if (ether2_surface == nullptr) return;

        vector<uint8_t> buff(sizeof(ethernet2_header) + data.size()/* + 4*/);

        // ethernet header
        write_in_network_order(buff.data(), &header);

        // encapsulated data
        memcpy(buff.data() + sizeof(struct ethernet2_header), data.data(), data.size());

        // crc
        // *(uint32_t*)(buff.data() + sizeof(struct ethernet2_header) + data.size()) = ethernet_calc_crc(buff.data(), sizeof(ethernet2_header) + data.size());

        ether2_surface->get_send_access()->send_data(buff);
    }

    void connect_recv(
        shared_ptr<net_access> const& ether2_surface, shared_ptr<recv_listener<pair<ethernet2_header, vector<uint8_t>>>> const& recv,
        optional<mac_addr> src_addr, optional<mac_addr> dest_addr, optional<ethertype> type
    ) {
        if (ether2_surface == nullptr) return;

        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<pair<ethernet2_header, vector<uint8_t>>>> recv;
            optional<mac_addr> src_addr;
            optional<mac_addr> dest_addr;
            optional<ethertype> type;
            my_recv(shared_ptr<recv_listener<pair<ethernet2_header, vector<uint8_t>>>> const& recv, optional<mac_addr> src_addr, optional<mac_addr> dest_addr, optional<ethertype> type) : recv(recv), src_addr(src_addr), dest_addr(dest_addr), type(type) {}

            void handle_recv(vector<uint8_t> const& data) override {
                ethernet2_header eth_header;
                extract_from_network_order(&eth_header, data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ethernet2_header), data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_addr.has_value()) {
                    if (eth_header.src_addr != src_addr.value()) {
                        // cout << "non matching src_addr, got " << mac_addr_to_str(eth_header.src_addr) << ", expected: " << mac_addr_to_str(src_addr.value()) << endl;
                        return;
                    }
                }

                if (dest_addr.has_value()) {
                    if (eth_header.dest_addr != dest_addr.value()) {
                        // cout << "non matching dest_addr, got " << mac_addr_to_str(eth_header.dest_addr) << ", expected: " << mac_addr_to_str(dest_addr.value()) << endl;
                        return;
                    }
                }

                if (type.has_value()) {
                    if (eth_header.prot != type.value()) return;
                }

                recv->handle_recv({eth_header, encap_data});
            }
        };

        ether2_surface->set_recv_access(make_shared<my_recv>(recv, src_addr, dest_addr, type));
    }

    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, optional<mac_addr> dest_addr, optional<ethertype> type, shared_ptr<net_access_generator_listener>&& listener) {
        struct my_recv : public recv_listener<pair<ethernet2_header, vector<uint8_t>>> {
            optional<mac_addr> dest_addr;
            optional<ethertype> type;
            multi_net_access multi_access;
            shared_ptr<net_access_generator_listener> listener;

            my_recv(optional<mac_addr> dest_addr, optional<ethertype> type, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_addr(dest_addr), type(type), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
            }

            void handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data) {
                // cout << "ether2 generator called" << endl;
                if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                // create a new net access
                auto new_net_access = make_shared<ethernet2_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_addr, dest_addr, type);

                // forward it to the generator
                listener->handle_new_net_access(new_net_access);
                
                // send the data
                // cout << "ether2 generator handles data" << endl;
                if (new_net_access->recv_access != nullptr) {
                    // cout << "ether2 generator sends data" << endl;
                    new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                }
            }
        };

        ethernet2_protocol::connect_recv(access, make_shared<my_recv>(dest_addr, type, access, std::move(listener)), {}, dest_addr, type);
    }
}
