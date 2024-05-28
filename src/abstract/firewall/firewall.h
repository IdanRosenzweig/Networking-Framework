#ifndef SERVERCLIENT_FIREWALL_H
#define SERVERCLIENT_FIREWALL_H

#include <thread>
#include <vector>
#include <iostream>
#include "basic_firewall_filter.h"
#include "../connection/connection.h"

// takes a base connection and applies various filters on incoming and outgoing packets.
// provides a connection api itself.
class firewall : public connection {
    connection * base;
public:
    explicit firewall(connection *base) : base(base) {
        base->add_listener(this);
    }

    ~firewall() {
        base->remove_listener(this);
    }

    std::vector<basic_firewall_filter<send_msg<>>*> outgoing_filters; // filters to apply on outgoing packets
    std::vector<basic_firewall_filter<received_msg>*> incoming_filters; // filters to apply to incoming packets

    int send_data(send_msg<> &&val) override { // todo fix the override of send_forwarder
        for (const auto& filter : outgoing_filters) {
            switch (filter->calc_policy(val)) {
                case ALLOW: break; // continue to next filter
                case BLOCK: {
//                    std::cout << "firewall blocked outgoing packet" << std::endl;
                    return 0; // stop
                }
                case DELAY: { // wait and continue
                    std::cout << "delaying outgoing packet" << std::endl;
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(1000ms);
                }
            }
        }

//        std::cout << "firewall accepted outgoing packet" << std::endl;
        return base->send_data(std::move(val));
    }

    void handle_received_event(received_msg &&event) override {
        for (const auto& filter : incoming_filters) {
            switch (filter->calc_policy(event)) {
                case ALLOW: break; // continue to next filter
                case BLOCK: {
//                    std::cout << "firewall blocked incoming packet" << std::endl;
                    return; // stop
                }
                case DELAY: { // wait and continue
                    std::cout << "delaying incoming packet" << std::endl;
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(1000ms);
                }
            }
        }

//        std::cout << "firewall accepted incoming packet" << std::endl;
        receive_forwarder::handle_received_event(std::move(event));
    }


};
#endif //SERVERCLIENT_FIREWALL_H
