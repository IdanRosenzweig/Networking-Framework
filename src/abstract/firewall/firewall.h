#ifndef NETWORKING_FIREWALL_H
#define NETWORKING_FIREWALL_H

#include <vector>
#include <thread>
#include <iostream>
#include "basic_firewall_filter.h"
#include "../connection/connection.h"

// takes a base connection and wraps it with various filters on incoming and outgoing buff.
// provides a connection api itself.
class firewall : public connection {
    connection *base;
public:
    explicit firewall(connection *base) : base(base) {
        base->add_listener(this);
    }

    ~firewall() {
        base->remove_listener(this);
    }

    std::vector<basic_firewall_filter<send_msg_t> *> outgoing_filters; // filters to apply on outgoing packets
    std::vector<basic_firewall_filter<recv_msg_t> *> incoming_filters; // filters to apply to incoming packets

    int send_data(send_msg_t &&data) override {
        for (const auto &filter: outgoing_filters) {
            switch (filter->calc_policy(data)) {
                case ALLOW:
                    break; // continue to next filter
                case BLOCK: {
//                    std::cout << "firewall blocked outgoing packet" << std::endl;
                    return 0; // stop
                }
                case DELAY: { // wait and continue
                    std::cout << "delaying outgoing packet" << std::endl;
                    std::this_thread::sleep_for(1000ms);
                }
            }
        }

//        std::cout << "firewall accepted outgoing packet" << std::endl;
        return base->send_data(std::move(data));
    }

    void handle_callback(recv_msg_t &&data) override {
        for (const auto &filter: incoming_filters) {
            switch (filter->calc_policy(data)) {
                case ALLOW:
                    break; // continue to next filter
                case BLOCK: {
//                    std::cout << "firewall blocked incoming packet" << std::endl;
                    return; // stop
                }
                case DELAY: { // wait and continue
                    std::cout << "delaying incoming packet" << std::endl;
                    std::this_thread::sleep_for(1000ms);
                }
            }
        }

//        std::cout << "firewall accepted incoming packet" << std::endl;
        recv_forwarder::handle_callback(std::move(data));
    }


};

#endif //NETWORKING_FIREWALL_H
