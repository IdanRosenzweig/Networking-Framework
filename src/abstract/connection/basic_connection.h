#ifndef SERVERCLIENT_BASIC_CONNECTION_H
#define SERVERCLIENT_BASIC_CONNECTION_H

#include "../sending/multi_sender.h"
#include "../receiving/multi_receiver.h"
#include "../sniffer/basic_sniffer.h"
#include <vector>

// a basic connection that you can send data on and listen to data from
// you can also attack sniffers to the connection, when data is sent or received

template <typename SEND_T, typename RECV_T>
class basic_connection : public multi_sender<SEND_T>, public multi_receiver<RECV_T> {
    std::vector<basic_sniffer<SEND_T, RECV_T>*> sniffers;
public:
    void attach_sniffer(basic_sniffer<SEND_T, RECV_T>* sniffer) {
        sniffers.push_back(sniffer);
    }

public:
    virtual ~basic_connection() {}

    int send_data(SEND_T& val) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(val);

        return multi_sender<SEND_T>::send_data(val);
    }

    void handle_received_event(RECV_T &event) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(event);

        multi_receiver<RECV_T>::handle_received_event(event);
    }

};

#endif //SERVERCLIENT_BASIC_CONNECTION_H
