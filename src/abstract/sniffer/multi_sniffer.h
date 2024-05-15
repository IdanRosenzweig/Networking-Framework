#ifndef SERVERCLIENT_MULTI_SNIFFER_H
#define SERVERCLIENT_MULTI_SNIFFER_H

#include "basic_sniffer.h"
#include <vector>

template <typename SEND_T, typename RECV_T>
class multi_sniffer : public basic_sniffer<SEND_T, RECV_T> {
    std::vector<basic_sniffer<SEND_T, RECV_T>*> sniffers;
public:
    void add_sniffer(basic_sniffer<SEND_T, RECV_T>* sniffer) {
        sniffers.push_back(sniffer);
    }

    void handle_outgoing_packet(SEND_T &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(msg);
    }

    void handle_incoming_packet(RECV_T &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(msg);
    }

};
#endif //SERVERCLIENT_MULTI_SNIFFER_H
