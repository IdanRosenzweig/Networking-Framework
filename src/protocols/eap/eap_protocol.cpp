#include "eap_protocol.h"

#include <cstring>
using namespace std;

namespace eap_protocol {
    void send(shared_ptr<net_access> const& eap_surface, eap_header const& header, vector<uint8_t> const& data) {
        if (eap_surface == nullptr) return;

        uint8_t big_buff[0x1000];

        // eap header
        auto header_sz = write_in_network_order(big_buff, &header);

        // encapsulated data
        memcpy(big_buff + header_sz, data.data(), data.size());

        eap_surface->get_send_access()->send_data(vector<uint8_t>(big_buff, big_buff + header_sz + data.size()));
    }

    // shared_ptr<send_medium_bytes> create_send_access_from_routing_table(eap_routing_table&& routing_table) {
    //     // todo take shared_ptr<routing_table> so that the table can be updated online
    //     struct my_send : public send_medium_bytes {
    //         eap_routing_table routing_table;
    //         my_send(eap_routing_table&& routing_table) : routing_table(std::move(routing_table)) {
    //         }

    //         err_t send_data(vector<uint8_t> const& data) override {
    //             struct eap_header header;
    //             extract_from_network_order(&header, data.data());

    //             auto route = routing_table.find_route(header.dest_addr);
    //             if (route == nullptr) return err_t::ERR;

    //             return route->get_send_access()->send_data(data);
    //         }
    //     };

    //     return make_shared<my_send>(std::move(routing_table));
    // }

    // void send(eap_routing_table const& routing_table, eap_header const& header, vector<uint8_t> const& data) {
    //     eap_protocol::send(routing_table.find_route(header.dest_addr), header, data);
    // }

    void connect_recv(
        shared_ptr<net_access> const& eap_surface, shared_ptr<recv_listener<pair<eap_header, vector<uint8_t>>>> const& recv,
        optional<eap_addr> src_addr, optional<eap_addr> dest_addr, optional<eap_prot_values> prot
    ) {
        if (eap_surface == nullptr) return;

        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<pair<eap_header, vector<uint8_t>>>> recv;
            optional<eap_addr> src_addr;
            optional<eap_addr> dest_addr;
            optional<eap_prot_values> prot;
            my_recv(shared_ptr<recv_listener<pair<eap_header, vector<uint8_t>>>> const& recv, optional<eap_addr> src_addr, optional<eap_addr> dest_addr, optional<eap_prot_values> prot) : recv(recv), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

            void handle_recv(vector<uint8_t> const& data) override {
                struct eap_header header;
                auto header_sz = extract_from_network_order(&header, data.data());

                vector<uint8_t> encap_data(data.data() + header_sz, data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_addr.has_value()) {
                    if (header.src_addr != src_addr.value()) {
                        // cout << "src addr no match, needed " << eap_addr_to_str(src_addr.value()) << " got " << eap_addr_to_str(header.src_addr) << endl;
                        return;
                    }
                }

                if (dest_addr.has_value()) {
                    if (header.dest_addr != dest_addr.value()) {
                        // cout << "dest addr no match" << endl;
                        return;
                    }
                }

                if (prot.has_value()) {
                    if (header.prot != prot.value()) {
                        // cout << "prot no match" << endl;
                        return;
                    }
                }

                recv->handle_recv({header, encap_data});
            }
        };

        eap_surface->set_recv_access(make_shared<my_recv>(recv, src_addr, dest_addr, prot));
    }

    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, optional<eap_addr> dest_addr, optional<eap_prot_values> prot, shared_ptr<net_access_generator_listener>&& listener) {
        struct my_recv : public recv_listener<pair<eap_header, vector<uint8_t>>> {
            optional<eap_addr> dest_addr;
            optional<eap_prot_values> prot;
            multi_net_access multi_access;
            shared_ptr<net_access_generator_listener> listener;

            my_recv(optional<eap_addr> dest_addr, optional<eap_prot_values> prot, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_addr(dest_addr), prot(prot), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
            }

            void handle_recv(pair<eap_header, vector<uint8_t>> const& data) {
                // cout << "eap generator called" << endl;
                if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                // create a new net access
                auto new_net_access = make_shared<eap_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_addr, dest_addr, prot);

                // forward it to the generator
                listener->handle_new_net_access(new_net_access);
                
                // send the data
                // cout << "eap generator handles data" << endl;
                if (new_net_access->recv_access != nullptr) {
                    // cout << "eap generator sends data" << endl;
                    new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                }
            }
        };

        eap_protocol::connect_recv(access, make_shared<my_recv>(dest_addr, prot, access, std::move(listener)), {}, dest_addr, prot);
    }
}
