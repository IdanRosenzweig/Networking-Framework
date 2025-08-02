#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <cstdint>
using namespace std;

#include "../receiving/basic_recv_listener.h"
#include "../sending/basic_send_medium.h"
#include "src/abstract/network_access/net_access_bytes.h"

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

struct sniffer_send : public basic_send_medium<vector<uint8_t>> {
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

struct sniffer_recv : public basic_recv_listener<vector<uint8_t>> {
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

struct sniffer_net_access : public net_access_bytes {
public:
    shared_ptr<net_access_bytes> net_access;

    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers_send; // handlers to use when sending
    vector<shared_ptr<basic_sniff_handler<vector<uint8_t>>>> handlers_recv; // handlers to use when receiving

    shared_ptr<basic_recv_listener<vector<uint8_t>>> recv;
    shared_ptr<basic_send_medium<vector<uint8_t>>> send;

public:
    sniffer_net_access(shared_ptr<net_access_bytes>&& _net_access) : net_access(std::move(_net_access)) {
        // receiving
        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
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

        net_access->set_recv_access(make_shared<my_recv>(this));

        // sending
        struct my_send : public basic_send_medium<vector<uint8_t>> {
        private:
            sniffer_net_access* par;

        public:
            my_send(sniffer_net_access* par) : par(par) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                for (auto& handler: par->handlers_send) {
                    handler->handle_sniff(data);
                }

                return par->net_access->get_send_access()->send_data(data);
            }
        };
        send = make_shared<my_send>(this);
    }

protected:
    /* send access */
    shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
        return send;
    }

    /* recv access */
    void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
        this->recv = recv;
    }

};