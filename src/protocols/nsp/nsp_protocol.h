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

#include "nsp_header.h"

namespace nsp_protocol {
    void send(shared_ptr<net_access> const& nsp_surface, nsp_header const& header, vector<uint8_t> const& data);

    // connect a receiver to all nsp packets which match the conditions of src_addr, dest_addr, and prot
    void connect_recv(
        shared_ptr<net_access> const& nsp_surface, shared_ptr<recv_listener<pair<nsp_header, vector<uint8_t>>>> const& recv,
        optional<nsp_id> src_id, optional<nsp_id> dest_id
    );

    // net_access based on a dest id in received packets.
    // can specify optional source id to match in received packets.
    struct net_access_single : public net_access {
    public:
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;
        
        shared_ptr<net_access> my_net_access;

    public:
        net_access_single(shared_ptr<net_access>&& access, nsp_id src_id, nsp_id dest_id) : my_net_access(std::move(access)) {
            struct my_recv : public recv_listener<pair<nsp_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<nsp_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    // cout << "nsp single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            nsp_protocol::connect_recv(my_net_access, make_shared<my_recv>(this), src_id, dest_id);

            struct my_send : public send_medium_bytes {
                net_access_single* par;
                nsp_id src_id;
                nsp_id dest_id;
                my_send(net_access_single* par, nsp_id src_id, nsp_id dest_id) : par(par), src_id(src_id), dest_id(dest_id) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // nsp header
                    struct nsp_header header;
                    header.src_id = dest_id;
                    header.dest_id = src_id;

                    // cout << "nsp single sends, src_id " << endl;
                    nsp_protocol::send(par->my_net_access, header, data);

                    return err_t::OK; // todo fix this
                }
            };
            send_access = make_shared<my_send>(this, src_id, dest_id);

        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            recv_access = recv;
        }

    };

    // generates new net_access from received packets, based on the source id.
    void connect_net_access_generator_listener(shared_ptr<net_access> const& access, nsp_id dest_id, shared_ptr<net_access_generator_listener>&& listener);
}