#ifndef SERVERCLIENT_ICMP_PROTOCOL_H
#define SERVERCLIENT_ICMP_PROTOCOL_H

#include "icmp_packet.h"
#include "../../abstract/utils/circular_buffer.h"
#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/utils/multiplexer.h"
#include "../../abstract/utils/next_choice.h"

class icmp_protocol : public msg_sender, public msg_receiver {
public:

    // send
    msg_sender* gateway;
    next_choice<int> next_type;
    next_choice<int> next_code;
    int send_data(send_msg msg) override;

    circular_buffer<received_msg> raw_message_q;
    multiplexer<int, basic_receiver*> type_handlers;
    basic_receiver* default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;

    void ping();

};


#endif //SERVERCLIENT_ICMP_PROTOCOL_H
