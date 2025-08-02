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

#include "ethernet2_header.h"

namespace ethernet2_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ethernet2_header const& header, vector<uint8_t> const& data);

    // connect a receiver to all ethernet packets which match the conditions of src_addr, dest_addr, and type
    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>>> const& recv,
        optional<mac_addr> src_addr, optional<mac_addr> dest_addr, optional<ethertype> type
    );

    // net_access based on any source addr in received packets.
    // can specify optional dest_addr and protocol to match in received packets.
    struct net_access_broadcast : public net_access_bytes {
    public:
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

        shared_ptr<net_access_bytes> my_net_access;

    public:
        net_access_broadcast(shared_ptr<net_access_bytes>&& net_access, optional<mac_addr> dest_addr, optional<ethertype> prot) : my_net_access(net_access) {
            struct my_recv : public basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>> {
                net_access_broadcast* par;
                my_recv(net_access_broadcast* par) : par(par) {}

                void handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ethernet2_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), {}, dest_addr, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                net_access_broadcast* par;
                optional<mac_addr> dest_addr;
                optional<ethertype> prot;
                my_send(net_access_broadcast* par, optional<mac_addr> dest_addr, optional<ethertype> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    struct ethernet2_header eth_header;
                    eth_header.src_addr = dest_addr.has_value() ? dest_addr.value() : mac_addr_empty;
                    eth_header.dest_addr = mac_addr_broadcast;
                    eth_header.prot = prot.has_value() ? prot.value() : static_cast<ethertype>(0); // todo some ETHERTYPE_NONE

                    ethernet2_protocol::send(par->my_net_access, eth_header, data);

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
        mac_addr src_addr;
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;
        
        shared_ptr<net_access_bytes> my_net_access;

    public:
        net_access_single(shared_ptr<net_access_bytes>&& net_access, mac_addr src_addr, optional<mac_addr> dest_addr, optional<ethertype> prot) : my_net_access(std::move(net_access)), src_addr(src_addr) {
            struct my_recv : public basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    cout << "single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ethernet2_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_addr, dest_addr, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                net_access_single* par;
                mac_addr src_addr;
                optional<mac_addr> dest_addr;
                optional<ethertype> prot;
                my_send(net_access_single* par, mac_addr src_addr, optional<mac_addr> dest_addr, optional<ethertype> prot) : par(par), src_addr(src_addr), dest_addr(dest_addr), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    struct ethernet2_header eth_header;
                    eth_header.src_addr = dest_addr.has_value() ? dest_addr.value() : mac_addr_empty;
                    eth_header.dest_addr = src_addr;
                    eth_header.prot = prot.has_value() ? prot.value() : static_cast<ethertype>(0); // todo some ETHERTYPE_NONE

                    // cout << "single sends, src_addr " << mac_addr_to_str(src_addr) << " dest_addr " << mac_addr_to_str(dest_addr.has_value() ? dest_addr.value() : mac_addr_empty) << endl;
                    ethernet2_protocol::send(par->my_net_access, eth_header, data);

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
        net_access_generator_single(shared_ptr<net_access_bytes>&& net_access, optional<mac_addr> dest_addr, optional<ethertype> prot) : multi_access(std::move(net_access)) {
            struct my_recv : public basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>> {
                net_access_generator_single* par;
                optional<mac_addr> dest_addr;
                optional<ethertype> prot;
                my_recv(net_access_generator_single* par, optional<mac_addr> dest_addr, optional<ethertype> prot) : par(par), dest_addr(dest_addr), prot(prot) {}

                void handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data) {
                    if (par->listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                    // create a new net access
                    auto new_net_access = make_shared<ethernet2_protocol::net_access_single>(std::move(par->multi_access.generate_net_access()), data.first.src_addr, dest_addr, prot);

                    // forward it to the generator
                    par->listener->handle_new_net_access(new_net_access);
                    
                    // send the data
                    if (new_net_access->recv_access != nullptr) {
                        new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                    }
                }
            };

            main_net_access = multi_access.generate_net_access();
            ethernet2_protocol::connect_recv(main_net_access, make_shared<my_recv>(this, dest_addr, prot), {}, dest_addr, prot);

        }
    };
}
