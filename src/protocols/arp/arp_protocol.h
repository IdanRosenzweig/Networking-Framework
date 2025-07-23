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

#include "arp_header.h"


struct arp_protocol {
private:
    shared_ptr<net_access_bytes> net_access;

public:
    void set_net_access(shared_ptr<net_access_bytes> const& access) {
        net_access = access;

        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
            arp_protocol* par;
            my_recv(arp_protocol* par) : par(par) {}

            void handle_recv(vector<uint8_t> const& data) override {
                shared_ptr<arp_header> arp = make_shared<arp_header>();
                extract_from_network_order(arp.get(), data.data());

                if (par->listener_all != nullptr) {
                    par->listener_all->handle_recv(arp);
                }

                if (auto listener = par->listener_op[arp->op]; listener != nullptr) {
                    listener->handle_recv(arp);
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
    arp_header next_header;

    void send();

    /* receiving */
    shared_ptr<basic_recv_listener<shared_ptr<arp_header>>> listener_all;
    map<
        uint16_t,
        shared_ptr<basic_recv_listener<shared_ptr<arp_header>>>
    > listener_op;

};
