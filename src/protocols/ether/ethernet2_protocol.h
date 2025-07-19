#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include <map>
using namespace std;

#include "src/abstract/network_access/network_access.h"
#include "src/abstract/receiving/encap_prot_listener.h"

#include "ethernet_header.h"


struct ethernet2_protocol {
private:
    shared_ptr<network_access_bytes> net_access;

public:
    void set_net_access(shared_ptr<network_access_bytes> const& access) {
        net_access = access;

        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
            ethernet2_protocol* par;

            my_recv(ethernet2_protocol* par) : par(par) {}

            void handle_recv(vector<uint8_t> const& data) override {
                shared_ptr<ethernet_header> eth_header = make_shared<ethernet_header>();
                extract_from_network_order(eth_header.get(), data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ethernet_header), data.data() + data.size());

                if (par->listener_all != nullptr) {
                    par->listener_all->handle_recv({eth_header, encap_data});
                }

                if (auto listener = par->listener_src_addr[eth_header->source_addr]; listener != nullptr) {
                    listener->handle_recv({eth_header, encap_data});
                }

                if (auto listener = par->listener_dest_addr[eth_header->dest_addr]; listener != nullptr) {
                    listener->handle_recv({eth_header, encap_data});
                }

                if (auto listener = par->listener_prot[eth_header->ether_type]; listener != nullptr) {
                    listener->handle_recv({eth_header, encap_data});
                }

                for (auto& [cond, listener] : par->listener_conds) {
                    if (cond(eth_header))
                        listener->handle_recv({eth_header, encap_data});
                }
            }
        };

        net_access->set_recv_access(
            make_shared<my_recv>(
                this
            )
        );
    }

public:
    /* sending */
    // parameters
    optional<ethertype> next_protocol;
    optional<mac_addr> next_dest_addr;
    optional<mac_addr> next_source_addr;

    void send(vector<uint8_t> const& data);

    /* receiving */
    shared_ptr<encap_prot_listener<ethernet_header>> listener_all;
    map<
        mac_addr,
        shared_ptr<encap_prot_listener<ethernet_header>>
    > listener_src_addr;
    map<
        mac_addr,
        shared_ptr<encap_prot_listener<ethernet_header>>
    > listener_dest_addr;
    map<
        uint16_t,
        shared_ptr<encap_prot_listener<ethernet_header>>
    > listener_prot;
    vector<pair<
        function<bool(shared_ptr<ethernet_header>)>,
        shared_ptr<encap_prot_listener<ethernet_header>>
    >> listener_conds;

};

