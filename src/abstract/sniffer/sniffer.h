#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <cstdint>
using namespace std;

#include "../receiving/recv_listener.h"
#include "../sending/send_medium.h"
#include "src/abstract/net_access/net_access.h"

#include "basic_sniff_handler.h"

// // provides sub structs "outgoing" and "incoming" which you can attach sniff handlers into
// struct sniffer {

//     struct outgoing_handler : public recv_listener<recv_msg_t> {
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

//     struct incoming_handler : public recv_listener<recv_msg_t> {
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

struct sniffer_send : public send_medium_bytes {
private:
    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers; // handlers to use
public:

    void add_handler(shared_ptr<basic_sniff_handler<vector<uint8_t>>>&& handler) {
        if (handler == nullptr) return;
        handlers.push_back(std::move(handler));
    }

    err_t send_data(vector<uint8_t> const& data) override {
        for (auto& handler: handlers) {
            handler->handle_sniff(data);
        }

        return err_t::OK;
    }
};

struct sniffer_recv : public recv_listener_bytes {
private:
    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers; // handlers to use

public:
    void add_handler(shared_ptr<basic_sniff_handler<vector<uint8_t>>>&& handler) {
        if (handler == nullptr) return;
        handlers.push_back(std::move(handler));
    }

    void handle_recv(vector<uint8_t> const& data) override {
        for (auto& handler: handlers) {
            handler->handle_sniff(data);
        }
    }
};

struct sniffer_net_access : public net_access {
public:
    shared_ptr<net_access> access;

    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers_send; // handlers to use when sending
    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers_recv; // handlers to use when receiving

    shared_ptr<recv_listener_bytes> recv;
    shared_ptr<send_medium_bytes> send;

public:
    sniffer_net_access(shared_ptr<net_access>&& _net_access) : access(std::move(_net_access)) {
        // receiving
        struct my_recv : public recv_listener_bytes {
        private:
            sniffer_net_access* par;

        public:
            my_recv(sniffer_net_access* par) : par(par) {
            }

            void handle_recv(vector<uint8_t> const& data) override {
                for (auto& handler: par->handlers_recv) {
                    handler->handle_sniff(data);
                }

                if (par->recv != nullptr) par->recv->handle_recv(data);
            }
        };

        access->set_recv_access(make_shared<my_recv>(this));

        // sending
        struct my_send : public send_medium_bytes {
        private:
            sniffer_net_access* par;

        public:
            my_send(sniffer_net_access* par) : par(par) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                for (auto& handler: par->handlers_send) {
                    handler->handle_sniff(data);
                }

                return par->access->get_send_access()->send_data(data);
            }
        };
        send = make_shared<my_send>(this);
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