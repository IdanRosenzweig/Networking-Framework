#ifndef SERVERCLIENT_BASIC_CONNECTION_H
#define SERVERCLIENT_BASIC_CONNECTION_H

#include "../sending/multi_sender.h"
#include "../sending/send_msg.h"
#include "../receiving/multi_receiver.h"
#include "../receiving/received_msg.h"
#include "basic_sniffer.h"

// a basic connection that you can send data on and listen to data from
class basic_connection : public multi_sender<send_msg>, public multi_receiver<received_msg> {
    vector<basic_sniffer*> sniffers;
public:
    void add_sniffer(basic_sniffer* sniffer) {
        sniffers.push_back(sniffer);
    }

public:
    virtual ~basic_connection() {}

    int send_data(send_msg val) override {
        received_msg msg;
        msg.data = unique_ptr<uint8_t>( new uint8_t[val.count]);
        memcpy(msg.data.get(), val.buff, val.count);
        msg.sz = val.count;
        msg.curr_offset = 0;
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(msg);

        return multi_sender::send_data(val);
    }

    void handle_received_event(received_msg &event) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(event);

        multi_receiver::handle_received_event(event);
    }

};

#endif //SERVERCLIENT_BASIC_CONNECTION_H
