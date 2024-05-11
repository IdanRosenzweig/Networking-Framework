#ifndef SERVERCLIENT_MULTI_SNIFFER_H
#define SERVERCLIENT_MULTI_SNIFFER_H

#include "basic_sniffer.h"

class multi_sniffer : public basic_sniffer {
    vector<basic_sniffer*> sniffers;
public:
    void add_sniffer(basic_sniffer* sniffer) {
        sniffers.push_back(sniffer);
    }

    void handle_outgoing_packet(received_msg &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(msg);
    }

    void handle_incoming_packet(received_msg &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(msg);
    }

};
#endif //SERVERCLIENT_MULTI_SNIFFER_H
