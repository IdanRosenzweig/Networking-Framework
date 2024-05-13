#ifndef SERVERCLIENT_IP4_PROTOCOL_H
#define SERVERCLIENT_IP4_PROTOCOL_H

#include <map>

#include "ip4_addr.h"
#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"

class ip4_protocol : public msg_sender, public msg_receiver {
public:

    ip4_protocol();

    // send
    next_choice<int> next_protocol;
    next_choice<uint8_t> next_ttl;
    next_choice<ip4_addr> next_dest_addr;
    next_choice<ip4_addr> next_source_addr;

    msg_sender *gateway;

    int send_data(send_msg msg) override;

    // recv
    multiplexer<int, basic_receiver *> protocol_handlers;
    basic_receiver * default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;

};

#endif //SERVERCLIENT_IP4_PROTOCOL_H
