#ifndef SERVERCLIENT_FIREWALL_H
#define SERVERCLIENT_FIREWALL_H

#include <thread>
#include <vector>
#include <iostream>
#include "basic_firewall_filter.h"
#include "../connection/msg_connection.h"

class firewall : public msg_connection {
    msg_connection * base;
public:
    explicit firewall(msg_connection *base) : base(base) {
        base->add_listener(this);
    }

    std::vector<basic_firewall_filter<send_msg<>>*> outgoing_filters; // filters to apply on outgoing packets
    std::vector<basic_firewall_filter<received_msg>*> incoming_filters; // filters to apply to incoming packets

    int send_data(send_msg<> &val) override { // todo fix the override of multi_sender
        for (const auto& filter : outgoing_filters) {
            // create a copy of the message and pass it to the filter
            send_msg<> msg_copy = val;
            switch (filter->calc_policy(msg_copy)) {
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
        return base->send_data(val);
    }

    void handle_received_event(received_msg &event) override {
        for (const auto& filter : incoming_filters) {
            // create a copy of the message and pass it to the filter
            received_msg msg_copy = event;
            switch (filter->calc_policy(msg_copy)) {
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
        multi_receiver::handle_received_event(event);
    }


};
#endif //SERVERCLIENT_FIREWALL_H
