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

#include "eap_header.h"
// #include "eap_routing_table.h"

namespace eap_protocol {
    void send(shared_ptr<net_access> const& eap_surface, eap_header const& header, vector<uint8_t> const& data);

    // shared_ptr<send_medium_bytes> create_send_access_from_routing_table(eap_routing_table&& routing_table);

    // connect a receiver to all eap packets which match the conditions of src_addr, dest_addr, and prot
    void connect_recv(
        shared_ptr<net_access> const& eap_surface, shared_ptr<recv_listener<pair<eap_header, vector<uint8_t>>>> const& recv,
        optional<eap_addr> src_addr, optional<eap_addr> dest_addr, optional<eap_prot_values> prot
    );

    // net_access based on any source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_broadcast : public net_access {
    public:
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;

        shared_ptr<net_access> my_net_access;

    public:
        net_access_broadcast(shared_ptr<net_access>&& net_access, optional<eap_addr> dest_addr, optional<eap_prot_values> prot) : my_net_access(net_access) {
            struct my_recv : public recv_listener<pair<eap_header, vector<uint8_t>>> {
                net_access_broadcast* par;
                my_recv(net_access_broadcast* par) : par(par) {}

                void handle_recv(pair<eap_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            eap_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), {}, dest_addr, prot);

            struct my_send : public send_medium_bytes {
                net_access_broadcast* par;
                optional<eap_addr> dest_addr;
                optional<eap_prot_values> prot;
                my_send(net_access_broadcast* par, optional<eap_addr> dest_addr, optional<eap_prot_values> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // eap header
                    struct eap_header header;
                    header.src_addr = dest_addr.has_value() ? dest_addr.value() : eap_addr_empty;
                    header.dest_addr = eap_addr_broadcast;

                    header.no_jumps = 0;

                    header.prot = prot.has_value() ? prot.value() : static_cast<eap_prot_values>(0); // todo some EMP_PROT_VALUES_NONE

                    eap_protocol::send(par->my_net_access, header, data);

                    return err_t::OK;// todo fix this
                }
            };
            send_access = make_shared<my_send>(this, dest_addr, prot);

        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            recv_access = recv;
        }

    };

    // net_access based on a single source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_single : public net_access {
    public:
        eap_addr src_addr;
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;
        
        shared_ptr<net_access> my_net_access;

    public:
        net_access_single(shared_ptr<net_access>&& net_access, eap_addr src_addr, optional<eap_addr> dest_addr, optional<eap_prot_values> prot) : my_net_access(std::move(net_access)), src_addr(src_addr) {
            struct my_recv : public recv_listener<pair<eap_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<eap_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    // cout << "eap single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            eap_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_addr, dest_addr, prot);

            struct my_send : public send_medium_bytes {
                net_access_single* par;
                eap_addr src_addr;
                optional<eap_addr> dest_addr;
                optional<eap_prot_values> prot;
                my_send(net_access_single* par, eap_addr src_addr, optional<eap_addr> dest_addr, optional<eap_prot_values> prot) : par(par), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // eap header
                    struct eap_header header;

                    header.src_addr = dest_addr.has_value() ? dest_addr.value() : eap_addr_empty;
                    header.dest_addr = src_addr;

                    header.no_jumps = 0;

                    header.prot = prot.has_value() ? prot.value() : static_cast<eap_prot_values>(0); // todo some IP_PROT_VALUES_NONE

                    // cout << "single sends, src_addr " << eap_addr_to_str(src_addr) << " dest_addr " << eap_addr_to_str(dest_addr.has_value() ? dest_addr.value() : eap_addr_empty) << endl;
                    eap_protocol::send(par->my_net_access, header, data);

                    return err_t::OK; // todo fix this
                }
            };
            send_access = make_shared<my_send>(this, src_addr, dest_addr, prot);

        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            recv_access = recv;
        }

    };

    // generates new net_access from received packets, based on the source addr.
    // can specify optional dest_addr and protocol to match in received packets.
    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, optional<eap_addr> dest_addr, optional<eap_prot_values> prot, shared_ptr<net_access_generator_listener>&& listener);

}
