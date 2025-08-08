#pragma once

#include <vector>
#include <thread>
#include <iostream>
using namespace std;

#include "basic_firewall_filter.h"

// #include "../connection/connection.h"

// // takes a base connection and wraps it with various filters on incoming and outgoing buff.
// // provides a connection api itself.
// class firewall : public connection {
//     connection *base;
// public:
//     explicit firewall(connection *base) : base(base) {
//         base->add_listener(this);
//     }

//     ~firewall() {
//         base->remove_listener(this);
//     }

//     std::vector<basic_firewall_filter<send_msg_t> *> outgoing_filters; // filters to apply on outgoing packets
//     std::vector<basic_firewall_filter<recv_msg_t> *> incoming_filters; // filters to apply to incoming packets

//     int send_data(send_msg_t &&data) override {
//         for (const auto &filter: outgoing_filters) {
//             switch (filter->calc_policy(data)) {
//                 case ALLOW:
//                     break; // continue to next filter
//                 case BLOCK: {
// //                    std::cout << "firewall blocked outgoing packet" << std::endl;
//                     return 0; // stop
//                 }
//                 case DELAY: { // wait and continue
//                     std::cout << "delaying outgoing packet" << std::endl;
//                     std::this_thread::sleep_for(1000ms);
//                 }
//             }
//         }

// //        std::cout << "firewall accepted outgoing packet" << std::endl;
//         return base->send_data(std::move(data));
//     }

//     void handle_callback(recv_msg_t &&data) override {
//         for (const auto &filter: incoming_filters) {
//             switch (filter->calc_policy(data)) {
//                 case ALLOW:
//                     break; // continue to next filter
//                 case BLOCK: {
// //                    std::cout << "firewall blocked incoming packet" << std::endl;
//                     return; // stop
//                 }
//                 case DELAY: { // wait and continue
//                     std::cout << "delaying incoming packet" << std::endl;
//                     std::this_thread::sleep_for(1000ms);
//                 }
//             }
//         }

// //        std::cout << "firewall accepted incoming packet" << std::endl;
//         recv_forwarder::handle_callback(std::move(data));
//     }


// };

template <typename T>
struct firewall_send : public send_medium<T> {
private:
    shared_ptr<send_medium<T>> send;

    vector<shared_ptr<basic_firewall_filter<T>>> filters; // filters to apply

public:
    explicit firewall(shared_ptr<send_medium<T>>&& _send) : send(std::move(_send)) {
    }

    void add_filter(shared_ptr<basic_firewall_filter<T>>&& filter) {
        if (filter == nullptr) return;
        filters.push_back(std::move(filter));
    }
    
    err_t send_data(T const& data) override {
        for (auto& filter: filters) {
            switch (filter->calc_policy(data)) {
                case firewall_policy::ALLOW:
                    break; // continue to next filter
                case firewall_policy::BLOCK: {
                    return err_t::ERR; // stop
                }
                case firewall_policy::DELAY: {
                    std::this_thread::sleep_for(1000ms); // todo include this metadata
                    break; // continue to next filter
                }
            }
        }

        return send->send_data(data);
    }
};

template <typename T>
struct firewall_recv : public recv_listener<T> {
private:
    shared_ptr<recv_listener<T>> recv;

    vector<shared_ptr<basic_firewall_filter<T>>> filters; // filters to apply

public:
    explicit firewall(shared_ptr<recv_listener<T>>&& _recv) : recv(std::move(_recv)) {
    }

    void add_filter(shared_ptr<basic_firewall_filter<T>>&& filter) {
        if (filter == nullptr) return;
        filters.push_back(std::move(filter));
    }

    void handle_recv(T const& data) override {
        for (auto& filter: filters) {
            switch (filter->calc_policy(data)) {
                case firewall_policy::ALLOW:
                    break; // continue to next filter
                case firewall_policy::BLOCK: {
                    return; // stop
                }
                case firewall_policy::DELAY: {
                    std::this_thread::sleep_for(1000ms); // todo include this metadata
                    break; // continue to next filter
                }
            }
        }

        recv->handle_recv(data);
    }
};

struct firewall_net_access : public net_access {
public:
    shared_ptr<net_access> net_access;

    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> filters_send; // filters to use when sending
    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> filters_recv; // filters to use when receiving

    shared_ptr<recv_listener_bytes> recv;
    shared_ptr<send_medium_bytes> send;

public:
    sniffer_net_access(shared_ptr<net_access>&& _net_access) : net_access(std::move(_net_access)) {
        // receiving
        struct my_recv : public recv_listener_bytes {
        private:
            sniffer_net_access* par;

        public:
            my_recv(sniffer_net_access* par) : par(par) {
            }

            void handle_recv(vector<uint8_t> const& data) override {
                for (auto& filter: par->filters_recv) {
                    switch (filter->calc_policy(data)) {
                        case firewall_policy::ALLOW:
                            break; // continue to next filter
                        case firewall_policy::BLOCK: {
                            return; // stop
                        }
                        case firewall_policy::DELAY: {
                            std::this_thread::sleep_for(1000ms); // todo include this metadata
                            break; // continue to next filter
                        }
                    }
                }

                if (par->recv != nullptr) par->recv->handle_recv(data);
            }
        };

        net_access->set_recv_access(make_shared<my_recv>(this));

        // sending
        struct my_send : public send_medium_bytes {
        private:
            sniffer_net_access* par;

        public:
            my_send(sniffer_net_access* par) : par(par) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                for (auto& filter: par->filters_send) {
                    switch (filter->calc_policy(data)) {
                        case firewall_policy::ALLOW:
                            break; // continue to next filter
                        case firewall_policy::BLOCK: {
                            return err_t::ERR; // stop
                        }
                        case firewall_policy::DELAY: {
                            std::this_thread::sleep_for(1000ms); // todo include this metadata
                            break; // continue to next filter
                        }
                    }
                }

                return par->net_access->get_send_access()->send_data(data);
            }
        };
        send = make_shared<my_send>(this);
    }

    void add_filter_send(shared_ptr<basic_firewall_filter<vector<uint8_t>>> const& filter) {
        if (filter == nullptr) return;
        filters_send.push_back(filter);
    }

    void add_filter_recv(shared_ptr<basic_firewall_filter<vector<uint8_t>>> const& filter) {
        if (filter == nullptr) return;
        filters_recv.push_back(filter);
    }
    
protected:
    /* send access */
    shared_ptr<send_medium_bytes> impl_get_send_access() override {
        return send;
    }

    /* recv access */
    void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
        this->recv = recv;
    }

};