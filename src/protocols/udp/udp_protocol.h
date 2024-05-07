#ifndef SERVERCLIENT_UDP_PROTOCOL_H
#define SERVERCLIENT_UDP_PROTOCOL_H

#include <map>

#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"

class udp_protocol : public msg_sender, public msg_receiver {
public:

    // send
    next_choice<int> next_source_port;
    next_choice<int> next_dest_port;

    msg_sender* gateway;

    int send_data(send_msg msg) override;

    // recv
    multiplexer<int, basic_receiver*> port_handlers;
    basic_receiver* default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;

};

#endif //SERVERCLIENT_IP4_PROTOCOL_H
