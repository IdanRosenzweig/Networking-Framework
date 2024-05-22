#ifndef SERVERCLIENT_SNIFF_MULTIPLEXER_H
#define SERVERCLIENT_SNIFF_MULTIPLEXER_H

#include "basic_sniffer.h"
#include <vector>

template <typename SEND_T, typename RECV_T>
class sniff_multiplexer : public basic_sniffer<SEND_T, RECV_T> {
    std::vector<basic_sniffer<SEND_T, RECV_T>*> sniffers;
public:
    void add_sniffer(basic_sniffer<SEND_T, RECV_T>* sniffer) {
        sniffers.push_back(sniffer);
    }

    void handle_outgoing_packet(const SEND_T &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(msg);
    }

    void handle_incoming_packet(const RECV_T &msg) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(msg);
    }

};
#endif //SERVERCLIENT_SNIFF_MULTIPLEXER_H
