#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include <map>
#include <iostream>
using namespace std;

#include "src/abstract/net_access/net_access.h"
#include "src/abstract/net_access/net_access_generator.h"
#include "src/abstract/net_access/multi_net_access.h"

#include "udp_header.h"

// class udp_protocol : public msg_send_medium, public msg_recv_listener {
// public:

//     udp_protocol();

//     // send
//     next_choice<uint16_t> next_source_port;
//     next_choice<uint16_t> next_dest_port;
//     msg_send_forwarder send_medium;

//     int send_data(send_msg_t &&data) override;

//     // recv
// #define UDP_LISTEN_ON_DEST_PORT_INDEX 0
//     multiplexer<recv_listener *,
//             uint16_t // dest port, index 0
//     > listeners;
//     recv_listener *default_listener = nullptr;

//     void handle_callback(recv_msg_t &&data) override;

// };

namespace udp_protocol {
    void send(shared_ptr<net_access> const& udp_surface, udp_header const& header, vector<uint8_t> const& data);

    // connect a receiver to all ip4 packets which match the conditions of src_addr, dest_addr, and prot
    void connect_recv(
        shared_ptr<net_access> const& udp_surface, shared_ptr<recv_listener<pair<udp_header, vector<uint8_t>>>> const& recv,
        optional<udp_port> src_port, optional<udp_port> dest_port
    );

    // net_access based on any source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    // struct net_access_broadcast : public net_access {
    // public:
    //     shared_ptr<send_medium_bytes> send_access;
    //     shared_ptr<recv_listener_bytes> recv_access;

    //     shared_ptr<net_access> my_net_access;

    // public:
    //     net_access_broadcast(shared_ptr<net_access>&& net_access, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : my_net_access(net_access) {
    //         struct my_recv : public recv_listener<pair<ip4_header, vector<uint8_t>>> {
    //             net_access_broadcast* par;
    //             my_recv(net_access_broadcast* par) : par(par) {}

    //             void handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
    //                 // forward to the recv access
    //                 if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
    //             }
    //         };
    //         ip4_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), {}, dest_addr, prot);

    //         struct my_send : public send_medium_bytes {
    //             net_access_broadcast* par;
    //             optional<ip4_addr> dest_addr;
    //             optional<ip_prot_values> prot;
    //             my_send(net_access_broadcast* par, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

    //             err_t send_data(vector<uint8_t> const& data) override {
    //                 // ip4 header
    //                 struct ip4_header header;
    //                 header.header_len = 5; // no additional options
    //                 header.version = 4;
    //                 header.type_of_service = 0;
    //                 int ip_packet_len = sizeof(struct ip4_header) + data.size();
    //                 header.total_len = ip_packet_len;
    //                 header.id = 1234;
    //                 header.flags = 0;
    //                 header.frag_off = 0;
    //                 header.ttl = 255;
    //                 header.prot = prot.has_value() ? prot.value() : static_cast<ip_prot_values>(0); // todo some IP_PROT_VALUES_NONE
    //                 header.checksum = 0;
    //                 header.src_addr = dest_addr.has_value() ? dest_addr.value() : ip4_addr_empty;
    //                 header.dest_addr = ip4_addr_broadcast;

    //                 ip4_protocol::send(par->my_net_access, header, data);

    //                 return err_t::OK;// todo fix this
    //             }
    //         };
    //         send_access = make_shared<my_send>(this, dest_addr, prot);

    //     }

    // protected:
    //     shared_ptr<send_medium_bytes> impl_get_send_access() override {
    //         return send_access;
    //     }

    //     void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
    //         recv_access = recv;
    //     }

    // };

    // net_access based on a single source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_single : public net_access {
    public:
        optional<udp_port> src_port;
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;
        
        shared_ptr<net_access> my_net_access;

    public:
        net_access_single(shared_ptr<net_access>&& access, optional<udp_port> src_port, udp_port dest_port) : my_net_access(std::move(access)), src_port(src_port) {
            struct my_recv : public recv_listener<pair<udp_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<udp_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    // cout << "udp single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            udp_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_port, dest_port);

            struct my_send : public send_medium_bytes {
                net_access_single* par;
                optional<udp_port> src_port;
                udp_port dest_port;
                my_send(net_access_single* par, optional<udp_port> src_port, udp_port dest_port) : par(par), src_port(src_port), dest_port(dest_port) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // udp header
                    struct udp_header header;
                    header.src_port = dest_port;
                    header.dest_port = src_port.has_value() ? src_port.value() : udp_port(0);

                    // cout << "udp single sends, src_port " << endl;
                    udp_protocol::send(par->my_net_access, header, data);

                    return err_t::OK; // todo fix this
                }
            };
            send_access = make_shared<my_send>(this, src_port, dest_port);

        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            recv_access = recv;
        }

    };

    // generates new net_access from received packets, based on the source port.
    // can specify optional dest port to match in received packets.
    // struct net_access_generator_single : public net_access_generator {
    // public:
    //     multi_net_access multi_access;
    //     shared_ptr<net_access> main_net_access;

    // public:
    //     net_access_generator_single(shared_ptr<net_access>&& net_access, udp_port dest_port) : multi_access(std::move(net_access)) {
    //         struct my_recv : public recv_listener<pair<udp_header, vector<uint8_t>>> {
    //             net_access_generator_single* par;
    //             udp_port dest_port;
    //             my_recv(net_access_generator_single* par, udp_port dest_port) : par(par), dest_port(dest_port) {}

    //             void handle_recv(pair<udp_header, vector<uint8_t>> const& data) {
    //                 cout << "udp generator called" << endl;
    //                 if (par->listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

    //                 // create a new net access
    //                 auto new_net_access = make_shared<udp_protocol::net_access_single>(std::move(par->multi_access.generate_net_access()), data.first.src_port, dest_port);

    //                 // forward it to the generator
    //                 par->listener->handle_new_net_access(new_net_access);
                    
    //                 // send the data
    //                 cout << "udp generator handles data" << endl;
    //                 if (new_net_access->recv_access != nullptr) {
    //                     cout << "udp generator sends data" << endl;
    //                     new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
    //                 }
    //             }
    //         };

    //         main_net_access = multi_access.generate_net_access();
    //         udp_protocol::connect_recv(main_net_access, make_shared<my_recv>(this, dest_port), {}, dest_port);

    //     }
    // };
    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, udp_port dest_port, shared_ptr<net_access_generator_listener>&& listener);
}