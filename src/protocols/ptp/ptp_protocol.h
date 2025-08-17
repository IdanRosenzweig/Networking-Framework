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

#include "ptp_header.h"
#include "local_conn/ptp_local_conn_routing_table.h"

#include "lib/hexdump.h"

namespace ptp_protocol {
    // write the ptp header along with the encapsulated data into a buffer
    vector<uint8_t> write_packet(ptp_header const& header, vector<uint8_t> const& data);

    // send the ptp header along with the encapsulated data into the send access
    void send(shared_ptr<net_access_send> const& ptp_send_surface, ptp_header const& header, vector<uint8_t> const& data);

    // connect a receiver for all ptp packets in which the condition meets.
    // the receiver gets the ptp header and the encapsulated data
    void connect_recv(
        shared_ptr<net_access_recv> const& ptp_recv_surface, shared_ptr<recv_listener<pair<ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& recv,
        function<bool(ptp_header const&)> cond
    );

    // connect a receiver for all ptp packets in which the condition meets.
    // the receiver gets the full original data, the ptp header and the encapsulated data
    void connect_recv_with_original(
        shared_ptr<net_access_recv> const& ptp_recv_surface, shared_ptr<recv_listener<tuple<vector<uint8_t>/*full original data*/, ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& recv,
        function<bool(ptp_header const&)> cond
    );
}

namespace ptp_protocol {
    // net_access based on a list of actions.
    struct net_access_single : public net_access {
    public:
        ptp_action_list list;
        shared_ptr<send_medium_bytes> send_access;
        shared_ptr<recv_listener_bytes> recv_access;
        
        shared_ptr<net_access> my_net_access;

    public:
        net_access_single(shared_ptr<net_access>&& _net_access, ptp_action_list const& _list) : my_net_access(std::move(_net_access)), list(_list) {
            struct my_recv : public recv_listener<pair<ptp_header, vector<uint8_t>>> {
                net_access_single* par;
                my_recv(net_access_single* par) : par(par) {}

                void handle_recv(pair<ptp_header, vector<uint8_t>> const& data) {
                    // forward to the recv access
                    cout << "ptp single received something" << endl;
                    if (par->recv_access != nullptr) par->recv_access->handle_recv(data.second);
                }
            };
            ptp_protocol::connect_recv(my_net_access, make_shared<my_recv>(this),
                [](ptp_header const& header) -> bool {
                    return ptp_is_endpoint_end(header);
                }
            );

            struct my_send : public send_medium_bytes {
                net_access_single* par;
                ptp_action_list list;
                my_send(net_access_single* par, ptp_action_list const& _list) : par(par), list(_list) {}

                err_t send_data(vector<uint8_t> const& data) override {
                    // ptp header
                    ptp_header header;
                    header.actions = list;
                    header.curr_action = 0;
                    header.no_jumps = 0;

                    ++header.curr_action; // todo move from here to the ptp center?
                    ++header.no_jumps;

                    cout << "ptp net_access_single sending" << endl;
                    ptp_protocol::send(par->my_net_access, header, data);

                    return err_t::OK; // todo fix this
                }
            };
            send_access = make_shared<my_send>(this, list);

        }

        net_access_single(net_access_single const& other) = delete;
        net_access_single(net_access_single&& other) = delete;

        net_access_single& operator=(net_access_single const& other) = delete;
        net_access_single& operator=(net_access_single&& other) = delete;

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send_access;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            recv_access = recv;
        }

    };


    struct my_net_access_generator_listener : public recv_listener<pair<ptp_header, vector<uint8_t>>> {
    private:
        // optional<ip_prot_values> prot;
        shared_ptr<net_access_recv> recv_access;
        multi_net_access_send multi_send_access;
        shared_ptr<net_access_generator_listener> listener;

    public:
        my_net_access_generator_listener(shared_ptr<net_access_recv> const& _recv, shared_ptr<net_access_send> const& _send, shared_ptr<net_access_generator_listener>&& _listener) : recv_access(_recv), multi_send_access(std::move(shared_ptr<net_access_send>(_send))), listener(std::move(_listener)) {
        }

        my_net_access_generator_listener(my_net_access_generator_listener const& other) = delete;
        my_net_access_generator_listener(my_net_access_generator_listener&& other) = delete;

        my_net_access_generator_listener& operator=(my_net_access_generator_listener const& other) = delete;
        my_net_access_generator_listener& operator=(my_net_access_generator_listener&& other) = delete;

        void handle_recv(pair<ptp_header, vector<uint8_t>> const& data) {
            if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

            // create a new net access
            shared_ptr<net_access_recv> cpy = recv_access;
            auto new_net_access = make_shared<ptp_protocol::net_access_single>(
                net_access_make_from_send_recv(multi_send_access.generate_net_access_send(), std::move(cpy)),
                create_reverse_actions(data.first.actions)
            );

            // forward it to the generator
            listener->handle_new_net_access(new_net_access);
            
            // send the data
            // cout << "ptp generator handles data" << endl;
            if (new_net_access->recv_access != nullptr) {
                // cout << "ptp generator sends data" << endl;
                new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
            }
        }
    };
    void connect_net_access_generator_listener(
        shared_ptr<net_access_recv> const& ptp_recv_surface,
        shared_ptr<net_access_send> const& ptp_send_surface,
        // prot
        shared_ptr<net_access_generator_listener>&& listener
    );


    struct node {
    public:
        // multi_net_access multi_access;
        shared_ptr<net_access_recv> recv;

        // shared_ptr<recv_listener<pair<ptp_header, vector<uint8_t>>>> handler; // todo fix the memory alloc

        ptp_addr my_addr;
        // shared_ptr<net_access_generator_listener> generator_listener;

        shared_ptr<recv_listener_bytes> recv_endpoint_end;

    public:
        node(shared_ptr<net_access_recv>&& _recv) : recv(std::move(_recv)) {
            struct my_recv : public recv_listener<tuple<vector<uint8_t>, ptp_header, vector<uint8_t>>> {
                node* par;
                my_recv(node* par) : par(par) {}

                void handle_packet(tuple<vector<uint8_t>, ptp_header, vector<uint8_t>> data) {
                    ptp_header& header = get<1>(data);
                    ptp_action const& action = header.get_curr_action();
                    switch (action.type) {
                        case ptp_action_t::endpoint: {
                            ptp_endpoint endpoint = std::get<ptp_endpoint>(action.val);

                            if (endpoint.type == ptp_endpoint_t::start) {
                                // cout << "handling ptp endpoint start" << endl;

                                // just increment to the next action
                                ++header.curr_action;

                                handle_packet(data);

                                break;

                            } else if (endpoint.type == ptp_endpoint_t::end) {
                                // cout << "handling ptp endpoint end" << endl;

                                if (par->recv_endpoint_end != nullptr) {
                                    // cout << "forwarding to recv_endpoint_end" << endl;
                                    auto buff = ptp_protocol::write_packet(header, get<2>(data));
                                    par->recv_endpoint_end->handle_recv(buff);
                                }

                                // if (par->generator_listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

                                // // create a new net access
                                // auto new_net_access = make_shared<ptp_protocol::net_access_single>(std::move(par->multi_access.generate_net_access()), create_reverse_actions(header.actions));

                                // // forward it to the generator
                                // par->generator_listener->handle_new_net_access(new_net_access);
                                
                                // // send the data
                                // // cout << "ptp generator handles data" << endl;
                                // if (new_net_access->recv_access != nullptr) {
                                //     // cout << "ptp generator sends data" << endl;
                                //     new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
                                // }

                                break;

                            }
                            
                            break;
                        }
                        case ptp_action_t::local_conn: {
                            // cout << "handling ptp local conn" << endl;
                            ptp_local_conn local_conn = std::get<ptp_local_conn>(action.val);

                            if (local_conn.dest_addr == par->my_addr) {
                                // local conn done, increment to the next action
                                ++header.curr_action;

                                handle_packet(data);

                                break;

                            } else {
                                // todo route or something
                                break;
                            }

                            break;
                        }
                        case ptp_action_t::net_switch: {
                            // cout << "handling ptp net switch" << endl;
                            ptp_net_switch net_switch = std::get<ptp_net_switch>(action.val);

                            // todo
                            // if net_switch.src_id is correct,
                            // and i have a route for net_switch.dest_id
                            // then forward or something 

                            break;
                        }
                        default: return;
                    }
                }

                void handle_recv(tuple<vector<uint8_t>, ptp_header, vector<uint8_t>> const& data) {
                    // cout << "ptp center handler called" << endl;

                    // increment the no_jumps
                    ptp_header next_header = std::get<1>(data);
                    ++next_header.no_jumps;

                    handle_packet({get<0>(data), next_header, get<2>(data)});

                }
            };
            // handler = make_shared<my_recv>(this);

            // main_access = multi_access.generate_net_access();
            ptp_protocol::connect_recv_with_original(recv, make_shared<my_recv>(this), [](ptp_header const& header) -> bool {return true;});
        }

        shared_ptr<net_access_recv> get_recv_endpoint_end() {
            struct my_net_access_recv : public net_access_recv {
            private:
                node* par;

            public:
                my_net_access_recv(node* par) : par(par) {}

            protected:
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
                    par->recv_endpoint_end = recv;
                }
            };

            return make_shared<my_net_access_recv>(this); // todo make this static
        }

    };
}