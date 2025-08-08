#include "ip4_protocol.h"

#include "internet_checksum.h"

#include <cstring>
using namespace std;

namespace ip4_protocol {
    void send(shared_ptr<net_access> const& ip4_surface, ip4_header const& header, vector<uint8_t> const& data) {
        if (ip4_surface == nullptr) return;

        vector<uint8_t> buff(sizeof(ip4_header) + data.size());

        // ip4 header
        ip4_header tmp_header = header;
        tmp_header.checksum = 0;
        write_in_network_order(buff.data(), &tmp_header);

        // checksum
        tmp_header.checksum = internet_checksum(buff.data(), sizeof(struct ip4_header));
        write_in_network_order(buff.data(), &tmp_header); // todo something smarter here

        // encapsulated data
        memcpy(buff.data() + sizeof(struct ip4_header), data.data(), data.size());

        ip4_surface->get_send_access()->send_data(buff);
    }

    shared_ptr<send_medium_bytes> create_send_access_from_routing_table(ip4_routing_table&& routing_table) {
        // todo take shared_ptr<routing_table> so that the table can be updated online
        struct my_send : public send_medium_bytes {
            ip4_routing_table routing_table;
            my_send(ip4_routing_table&& routing_table) : routing_table(std::move(routing_table)) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                struct ip4_header header;
                extract_from_network_order(&header, data.data());

                auto route = routing_table.find_route(header.dest_addr);
                if (route == nullptr) return err_t::ERR;

                return route->get_send_access()->send_data(data);
            }
        };

        return make_shared<my_send>(std::move(routing_table));
    }

    void send(ip4_routing_table const& routing_table, ip4_header const& header, vector<uint8_t> const& data) {
        ip4_protocol::send(routing_table.find_route(header.dest_addr), header, data);
    }

    void connect_recv(
        shared_ptr<net_access> const& ip4_surface, shared_ptr<recv_listener<pair<ip4_header, vector<uint8_t>>>> const& recv,
        optional<ip4_addr> src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot
    ) {
        if (ip4_surface == nullptr) return;

        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<pair<ip4_header, vector<uint8_t>>>> recv;
            optional<ip4_addr> src_addr;
            optional<ip4_addr> dest_addr;
            optional<ip_prot_values> prot;
            my_recv(shared_ptr<recv_listener<pair<ip4_header, vector<uint8_t>>>> const& recv, optional<ip4_addr> src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : recv(recv), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

            void handle_recv(vector<uint8_t> const& data) override {
                ip4_header ip_header;
                extract_from_network_order(&ip_header, data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ip4_header), data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_addr.has_value()) {
                    if (ip_header.src_addr != src_addr.value()) {
                        // cout << "src addr no match, needed " << ip4_addr_to_str(src_addr.value()) << " got " << ip4_addr_to_str(ip_header.src_addr) << endl;
                        return;
                    }
                }

                if (dest_addr.has_value()) {
                    if (ip_header.dest_addr != dest_addr.value()) {
                        // cout << "dest addr no match" << endl;
                        return;
                    }
                }

                if (prot.has_value()) {
                    if (ip_header.prot != prot.value()) {
                        // cout << "prot no match" << endl;
                        return;
                    }
                }

                recv->handle_recv({ip_header, encap_data});
            }
        };

        ip4_surface->set_recv_access(make_shared<my_recv>(recv, src_addr, dest_addr, prot));
    }

    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot, shared_ptr<net_access_generator_listener>&& listener) {
        struct my_recv : public recv_listener<pair<ip4_header, vector<uint8_t>>> {
            optional<ip4_addr> dest_addr;
            optional<ip_prot_values> prot;
            multi_net_access multi_access;
            shared_ptr<net_access_generator_listener> listener;

            my_recv(optional<ip4_addr> dest_addr, optional<ip_prot_values> prot, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_addr(dest_addr), prot(prot), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
            }

            void handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
                // cout << "ip4 generator called" << endl;
                if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                // create a new net access
                auto new_net_access = make_shared<ip4_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_addr, dest_addr, prot);

                // forward it to the generator
                listener->handle_new_net_access(new_net_access);
                
                // send the data
                // cout << "ip4 generator handles data" << endl;
                if (new_net_access->recv_access != nullptr) {
                    // cout << "ip4 generator sends data" << endl;
                    new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                }
            }
        };

        ip4_protocol::connect_recv(access, make_shared<my_recv>(dest_addr, prot, access, std::move(listener)), {}, dest_addr, prot);
    }
}
