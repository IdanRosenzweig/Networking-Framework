#ifndef NETWORKING_IP4_PROTOCOL_H
#define NETWORKING_IP4_PROTOCOL_H

#include <map>

#include "ip4_addr.h"
#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"

class ip4_protocol : public msg_send_medium, public msg_recv_listener {
public:

    ip4_protocol();

    // send
    next_choice<int> next_protocol;
    next_choice<uint8_t> next_ttl;
    next_choice<ip4_addr> next_dest_addr;
    next_choice<ip4_addr> next_source_addr;

    msg_send_medium *send_medium;

    int send_data(send_msg<>&& msg) override;


    // recv
    map<int, vector<basic_recv_listener *>> protocol_handlers;
    basic_recv_listener * default_handler = nullptr;
    void handle_received_event(received_msg&& msg) override;

};

#endif //NETWORKING_IP4_PROTOCOL_H
