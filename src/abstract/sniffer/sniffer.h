#pragma once

#include <vector>
#include <iostream>
#include <memory>
using namespace std;

#include "../receiving/basic_recv_listener.h"
#include "../sending/basic_send_medium.h"

#include "basic_sniff_handler.h"

// // provides sub structs "outgoing" and "incoming" which you can attach sniff handlers into
// struct sniffer {

//     struct outgoing_handler : public basic_recv_listener<recv_msg_t> {
//     public:
//         std::vector<basic_sniff_handler*> sniffers; // sniffers to call for outgoing packets

//         void handle_callback(recv_msg_t &&data) override {
//             // send to all sniffers
//             for (auto& sniffer : sniffers) {
//                 recv_msg_t copy(data);
//                 sniffer->handle_callback(std::move(copy));
//             }
//         }
//     } outgoing;

//     struct incoming_handler : public basic_recv_listener<recv_msg_t> {
//     public:
//         std::vector<basic_sniff_handler*> sniffers; // sniffers to call for incoming packets

//         void handle_callback(recv_msg_t &&data) override {
//             // send to all sniffers
//             for (auto& sniffer : sniffers) {
//                 recv_msg_t copy(data);
//                 sniffer->handle_callback(std::move(copy));
//             }
//         }
//     } incoming;

// };

template <typename T>
struct sniffer_send : public basic_send_medium<T> {
private:
    shared_ptr<basic_send_medium<T>> send;

    vector<shared_ptr<basic_sniff_handler<T>>> handlers; // handlers to use

public:
    explicit sniffer_send(shared_ptr<basic_send_medium<T>> const& send) : send(send) {
    }

    void add_handler(shared_ptr<basic_sniff_handler<T>> const& handler) {
        if (handler == nullptr) return;
        handlers.push_back(handler);
    }

    int send_data(T const& data) override {
        for (auto& handler: handlers) {
            handler->handle_sniff(data);
        }

        return send->send_data(data);
    }
};

template <typename T>
struct sniffer_recv : public basic_recv_listener<T> {
private:
    shared_ptr<basic_recv_listener<T>> recv;

    vector<shared_ptr<basic_sniff_handler<T>>> handlers; // handlers to use

public:
    explicit sniffer_recv(shared_ptr<basic_recv_listener<T>> const& recv) : recv(recv) {
    }

    void add_handler(shared_ptr<basic_sniff_handler<T>> const& handler) {
        if (handler == nullptr) return;
        handlers.push_back(handler);
    }

    int handle_recv(T const& data) override {
        for (auto& handler: handlers) {
            handler->handle_sniff(data);
        }

        recv->handle_recv(data);
    }
};
