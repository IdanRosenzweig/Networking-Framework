#ifndef SERVERCLIENT_ICMP_PROTOCOL_H
#define SERVERCLIENT_ICMP_PROTOCOL_H

#include "icmp_packet.h"
#include "../../abstract/circular_buffer.h"
#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/multiplexer.h"

class icmp_protocol : public msg_sender, public msg_receiver {
public:

    msg_sender* gateway;

    void ping();

    int send_data(send_msg msg) override;

    circular_buffer<received_msg> raw_message_q;
    multiplexer<int, basic_receiver*> type_handlers;

    void handle_received_event(received_msg msg) override;
};


#endif //SERVERCLIENT_ICMP_PROTOCOL_H
