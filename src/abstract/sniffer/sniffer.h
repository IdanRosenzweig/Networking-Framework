#ifndef NETWORKING_SNIFFER_H
#define NETWORKING_SNIFFER_H

#include <vector>
#include <iostream>
#include "../receiving/basic_recv_listener.h"
#include "basic_sniff_handler.h"

// provides sub structs "outgoing" and "incoming" which you can attach sniff handlers into
struct sniffer {

    struct outgoing_handler : public basic_recv_listener<received_msg> {
    public:
        std::vector<basic_sniff_handler*> sniffers; // sniffers to call for outgoing packets

        void handle_received_event(received_msg &&event) override {
            // send to all sniffers
            for (auto& sniffer : sniffers) {
                received_msg copy(event);
                sniffer->handle_received_event(std::move(copy));
            }
        }
    } outgoing;

    struct incoming_handler : public basic_recv_listener<received_msg> {
    public:
        std::vector<basic_sniff_handler*> sniffers; // sniffers to call for incoming packets

        void handle_received_event(received_msg &&event) override {
            // send to all sniffers
            for (auto& sniffer : sniffers) {
                received_msg copy(event);
                sniffer->handle_received_event(std::move(copy));
            }
        }
    } incoming;

};

#endif //NETWORKING_SNIFFER_H
