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
struct firewall_send : public basic_send_medium<T> {
private:
    shared_ptr<basic_send_medium<T>> send;

    vector<shared_ptr<basic_firewall_filter<T>>> filters; // filters to apply

public:
    explicit firewall(shared_ptr<basic_send_medium<T>> const& send) : send(send) {
    }

    void add_filter(shared_ptr<basic_firewall_filter<T>> const& filter) {
        if (filter == nullptr) return;
        filters.push_back(filter);
    }
    
    int send_data(T const& data) override {
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
struct firewall_recv : public basic_recv_listener<T> {
private:
    shared_ptr<basic_recv_listener<T>> recv;

    vector<shared_ptr<basic_firewall_filter<T>>> filters; // filters to apply

public:
    explicit firewall(shared_ptr<basic_recv_listener<T>> const& recv) : recv(recv) {
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