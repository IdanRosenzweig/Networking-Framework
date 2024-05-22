#ifndef SERVERCLIENT_CONNECTION_H
#define SERVERCLIENT_CONNECTION_H

#include "../sending/send_multiplexer.h"
#include "../receiving/receive_multiplexer.h"
#include "../sniffer/basic_sniffer.h"
#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"
#include <vector>

// a basic connection that you can send data on and listen to data from.
// you can also attach sniffers to the connection, when data is sent or received
class connection : public send_multiplexer<send_msg<>>, public receive_multiplexer<received_msg> {
    std::vector<basic_sniffer<send_msg<>, received_msg>*> sniffers;
public:
    void attach_sniffer(basic_sniffer<send_msg<>, received_msg>* sniffer) {
        sniffers.push_back(sniffer);
    }

public:
    virtual ~connection() {}

    virtual int send_data(send_msg<>&& val) override {
        // send to all sniffers
        for (auto& sniffer : sniffers)
            sniffer->handle_outgoing_packet(val);

        return send_multiplexer<send_msg<>>::send_data(std::move(val));
    }

    virtual void handle_received_event(received_msg &&event) override {
        for (auto& sniffer : sniffers)
            sniffer->handle_incoming_packet(event);

        receive_multiplexer<received_msg>::handle_received_event(std::move(event));
    }

};

#endif //SERVERCLIENT_CONNECTION_H
