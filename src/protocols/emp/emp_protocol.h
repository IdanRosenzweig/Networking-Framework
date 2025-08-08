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

#include "emp_header.h"

namespace emp_protocol {
    void send(shared_ptr<net_access> const& emp_surface, emp_header const& header, vector<uint8_t> const& data);

    // connect a receiver to all emp packets which match the conditions of src_addr, dest_addr, and prot
    void connect_recv(
        shared_ptr<net_access> const& emp_surface, shared_ptr<recv_listener<pair<emp_header, vector<uint8_t>>>> const& recv,
        optional<emp_port> src_port, optional<emp_port> dest_port
    );

    // net_access based on any source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    // struct net_access_broadcast : public net_access {
    // public:
    //     shared_ptr<send_medium_bytes> send_access;
    //     shared_ptr<recv_listener_bytes> recv_access;

    //     shared_ptr<net_access> my_net_access;

    // public:
    //     net_access_broadcast(shared_ptr<net_access>&& net_access, optional<emp_addr> dest_addr, optional<ip_prot_values> prot) : my_net_access(net_access) {
    //         struct my_recv : public recv_listener<pair<emp_header, vector<uint8_t>>> {
    //             net_access_broadcast* par;
    //             my_recv(net_access_broadcast* par) : par(par) {}

    //             void handle_recv(pair<emp_header, vector<uint8_t>> const& data) {
    //                 // forward to the recv access
    //                 if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
    //             }
    //         };
    //         emp_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), {}, dest_addr, prot);

    //         struct my_send : public send_medium_bytes {
    //             net_access_broadcast* par;
    //             optional<emp_addr> dest_addr;
    //             optional<ip_prot_values> prot;
    //             my_send(net_access_broadcast* par, optional<emp_addr> dest_addr, optional<ip_prot_values> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

    //             err_t send_data(vector<uint8_t> const& data) override {
    //                 // emp header
    //                 struct emp_header header;
    //                 header.header_len = 5; // no additional options
    //                 header.version = 4;
    //                 header.type_of_service = 0;
    //                 int ip_packet_len = sizeof(struct emp_header) + data.size();
    //                 header.total_len = ip_packet_len;
    //                 header.id = 1234;
    //                 header.flags = 0;
    //                 header.frag_off = 0;
    //                 header.ttl = 255;
    //                 header.prot = prot.has_value() ? prot.value() : static_cast<ip_prot_values>(0); // todo some IP_PROT_VALUES_NONE
    //                 header.checksum = 0;
    //                 header.src_addr = dest_addr.has_value() ? dest_addr.value() : emp_addr_empty;
    //                 header.dest_addr = emp_addr_broadcast;

    //                 emp_protocol::send(par->my_net_access, header, data);

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
        optional<emp_port> src_port;
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;
        
        shared_ptr<net_access> my_net_access;

    public:
        net_access_single(shared_ptr<net_access>&& access, optional<emp_port> src_port, emp_port dest_port) : my_net_access(std::move(access)), src_port(src_port) {
            struct my_recv : public recv_listener<pair<emp_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<emp_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    // cout << "emp single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            emp_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_port, dest_port);

            struct my_send : public send_medium_bytes {
                net_access_single* par;
                optional<emp_port> src_port;
                emp_port dest_port;
                my_send(net_access_single* par, optional<emp_port> src_port, emp_port dest_port) : par(par), src_port(src_port), dest_port(dest_port) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // emp header
                    struct emp_header header;
                    header.src_port = dest_port;
                    header.dest_port = src_port.has_value() ? src_port.value() : emp_port_empty;

                    // cout << "emp single sends, src_port " << endl;
                    emp_protocol::send(par->my_net_access, header, data);

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
    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, emp_port dest_port, shared_ptr<net_access_generator_listener>&& listener);
}