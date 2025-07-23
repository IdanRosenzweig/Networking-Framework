#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include <map>
using namespace std;

#include "src/abstract/network_access/net_access_bytes.h"
#include "src/abstract/receiving/encap_prot_listener.h"

#include "ethernet_header.h"

namespace ethernet2_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ethernet_header const& header, vector<uint8_t> const& data);

    // connect a receiver to all ethernet packets which match the conditions of src_mac, dest_mac, and type
    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<encap_prot_listener<ethernet_header>> const recv,
        optional<mac_addr> src_mac, optional<mac_addr> dest_mac, optional<ethertype> type
    );

    struct net_access_broadcast : public net_access_bytes {
    public:
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

    public:
        net_access_broadcast(shared_ptr<net_access_bytes> const& net_access, mac_addr src_mac, optional<ethertype> prot) {
            struct my_recv : public encap_prot_listener<ethernet_header> {
                net_access_broadcast* par;
                my_recv(net_access_broadcast* par) : par(par) {}

                void handle_recv(pair<shared_ptr<ethernet_header>, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ethernet2_protocol::connect_recv(net_access, make_shared<my_recv>(this), {}, src_mac, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                shared_ptr<net_access_bytes> const& net_access;
                mac_addr src_mac;
                optional<ethertype> prot;
                my_send(shared_ptr<net_access_bytes> const& net_access, mac_addr src_mac, optional<ethertype> prot) : net_access(net_access), src_mac(src_mac), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    struct ethernet_header eth_header;
                    eth_header.source_addr = src_mac;
                    eth_header.dest_addr = mac_addr_broadcast;
                    eth_header.ether_type = prot.has_value() ? prot.value() : static_cast<ethertype>(0); // todo some ETHERTYPE_NONE

                    ethernet2_protocol::send(net_access, eth_header, data);

                    return err_t::OK;// todo fix this
                }
            };
            send_access = make_shared<my_send>(net_access, src_mac, prot);

        }

    protected:
        shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
            recv_access = recv;
        }

    };


    struct net_access_single : public net_access_bytes {
    public:
        shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
        shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

    public:
        net_access_single(shared_ptr<net_access_bytes> const& net_access, mac_addr target, mac_addr src_mac, optional<ethertype> prot) {
            struct my_recv : public encap_prot_listener<ethernet_header> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<shared_ptr<ethernet_header>, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ethernet2_protocol::connect_recv(net_access, make_shared<my_recv>(this), target, src_mac, prot);

            struct my_send : public basic_send_medium<vector<uint8_t>> {
                shared_ptr<net_access_bytes> const& net_access;
                mac_addr target;
                mac_addr src_mac;
                optional<ethertype> prot;
                my_send(shared_ptr<net_access_bytes> const& net_access, mac_addr target, mac_addr src_mac, optional<ethertype> prot) : net_access(net_access), target(target), src_mac(src_mac), prot(prot) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    struct ethernet_header eth_header;
                    eth_header.source_addr = src_mac;
                    eth_header.dest_addr = target;
                    eth_header.ether_type = prot.has_value() ? prot.value() : static_cast<ethertype>(0); // todo some ETHERTYPE_NONE

                    ethernet2_protocol::send(net_access, eth_header, data);

                    return err_t::OK;// todo fix this
                }
            };
            send_access = make_shared<my_send>(net_access, target, src_mac, prot);

        }

    protected:
        shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
            recv_access = recv;
        }

    };

}
