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

#include "src/abstract/network_access/net_access_bytes.h"
#include "src/abstract/network_access/net_access_generator.h"
#include "src/abstract/network_access/multi_net_access.h"

// #include "../../abstract/sending/msg/send_msg_t.h"
// #include "../../abstract/receiving/msg/recv_msg_t.h"
// #include "../../utils/next_choice.h"
// #include "../../utils/multiplexer.h"
#include "ip4_header.h"
#include "ip4_routing_table.h"

namespace ip4_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ip4_header const& header, vector<uint8_t> const& data);

    shared_ptr<net_access_bytes> create_net_access_from_routing_table(ip4_routing_table&& routing_table);

    // connect a receiver to all ip4 packets which match the conditions of src_addr, dest_addr, and prot
    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<basic_recv_listener<pair<ip4_header, vector<uint8_t>>>> const& recv,
        optional<ip4_addr> src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot
    );

    // net_access based on any source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_broadcast : public net_access_bytes {
    public:
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

        shared_ptr<net_access_bytes> my_net_access;

    public:
        net_access_broadcast(shared_ptr<net_access_bytes>&& net_access, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : my_net_access(net_access) {
            struct my_recv : public basic_recv_listener<pair<ip4_header, vector<uint8_t>>> {
                net_access_broadcast* par;
                my_recv(net_access_broadcast* par) : par(par) {}

                void handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ip4_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), {}, dest_addr, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                net_access_broadcast* par;
                optional<ip4_addr> dest_addr;
                optional<ip_prot_values> prot;
                my_send(net_access_broadcast* par, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // ip4 header
                    struct ip4_header header;
                    header.header_len = 5; // no additional options
                    header.version = 4;
                    header.type_of_service = 0;
                    int ip_packet_len = sizeof(struct ip4_header) + data.size();
                    header.total_len = ip_packet_len;
                    header.id = 1234;
                    header.flags = 0;
                    header.frag_off = 0;
                    header.ttl = 255;
                    header.prot = prot.has_value() ? prot.value() : static_cast<ip_prot_values>(0); // todo some IP_PROT_VALUES_NONE
                    header.checksum = 0;
                    header.src_addr = dest_addr.has_value() ? dest_addr.value() : ip4_addr_empty;
                    header.dest_addr = ip4_addr_broadcast;

                    ip4_protocol::send(par->my_net_access, header, data);

                    return err_t::OK;// todo fix this
                }
            };
            send_access = make_shared<my_send>(this, dest_addr, prot);

        }

    protected:
        shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
            recv_access = recv;
        }

    };

    // net_access based on a single source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_single : public net_access_bytes {
    public:
        ip4_addr src_addr;
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;
        
        shared_ptr<net_access_bytes> my_net_access;

    public:
        net_access_single(shared_ptr<net_access_bytes>&& net_access, ip4_addr src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : my_net_access(std::move(net_access)), src_addr(src_addr) {
            struct my_recv : public basic_recv_listener<pair<ip4_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    cout << "single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ip4_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_addr, dest_addr, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                net_access_single* par;
                ip4_addr src_addr;
                optional<ip4_addr> dest_addr;
                optional<ip_prot_values> prot;
                my_send(net_access_single* par, ip4_addr src_addr, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : par(par), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // ip4 header
                    struct ip4_header header;
                    header.header_len = 5; // no additional options
                    header.version = 4;
                    header.type_of_service = 0;
                    int ip_packet_len = sizeof(struct ip4_header) + data.size();
                    header.total_len = ip_packet_len;
                    header.id = 1234;
                    header.flags = 0;
                    header.frag_off = 0;
                    header.ttl = 255;
                    header.prot = prot.has_value() ? prot.value() : static_cast<ip_prot_values>(0); // todo some IP_PROT_VALUES_NONE
                    header.checksum = 0;
                    header.src_addr = dest_addr.has_value() ? dest_addr.value() : ip4_addr_empty;
                    header.dest_addr = src_addr;

                    // cout << "single sends, src_addr " << ip4_addr_to_str(src_addr) << " dest_addr " << ip4_addr_to_str(dest_addr.has_value() ? dest_addr.value() : ip4_addr_empty) << endl;
                    ip4_protocol::send(par->my_net_access, header, data);

                    return err_t::OK; // todo fix this
                }
            };
            send_access = make_shared<my_send>(this, src_addr, dest_addr, prot);

        }

    protected:
        shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
            recv_access = recv;
        }

    };

    // generates new net_access from received packets, based on the source addr.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_generator_single : public net_access_generator {
    public:
        multi_net_access multi_access;
        shared_ptr<net_access_bytes> main_net_access;

    public:
        net_access_generator_single(shared_ptr<net_access_bytes>&& net_access, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : multi_access(std::move(net_access)) {
            struct my_recv : public basic_recv_listener<pair<ip4_header, vector<uint8_t>>> {
                net_access_generator_single* par;
                optional<ip4_addr> dest_addr;
                optional<ip_prot_values> prot;
                my_recv(net_access_generator_single* par, optional<ip4_addr> dest_addr, optional<ip_prot_values> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

                void handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
                    if (par->listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                    // create a new net access
                    auto new_net_access = make_shared<ip4_protocol::net_access_single>(std::move(par->multi_access.generate_net_access()), data.first.src_addr, dest_addr, prot);

                    // forward it to the generator
                    par->listener->handle_new_net_access(new_net_access);
                    
                    // send the data
                    if (new_net_access->recv_access != nullptr) {
                        new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                    }
                }
            };

            main_net_access = multi_access.generate_net_access();
            ip4_protocol::connect_recv(main_net_access, make_shared<my_recv>(this, dest_addr, prot), {}, dest_addr, prot);

        }
    };
}

// class ip4_protocol : public msg_send_medium, public msg_recv_listener {
// public:

//     ip4_protocol();

//     // send
//     next_choice<uint8_t> next_protocol;
//     next_choice<uint8_t> next_ttl;
//     next_choice<ip4_addr> next_dest_addr;
//     next_choice<ip4_addr> next_source_addr;
//     msg_send_forwarder send_medium;

//     int send_data(send_msg_t &&data) override;


//     // recv
// #define IP4_LISTEN_ON_PROTOCOL_INDEX 0
// #define IP4_LISTEN_ON_SRC_ADDR_INDEX 1
// #define IP4_LISTEN_ON_DEST_ADDR_INDEX 2
//     multiplexer<basic_recv_listener *,
//             uint8_t, // protocol, index 0
//             ip4_addr, // src addr, index 1
//             ip4_addr // dest addr, index 2
//     > listeners;
//     basic_recv_listener *default_listener = nullptr;

//     void handle_callback(recv_msg_t &&data) override;

// };
