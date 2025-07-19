#ifndef NETWORKING_IP4_PROTOCOL_H
#define NETWORKING_IP4_PROTOCOL_H

#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../utils/next_choice.h"
#include "../../utils/multiplexer.h"
#include "ip4_header.h"

class ip4_protocol : public msg_send_medium, public msg_recv_listener {
public:

    ip4_protocol();

    // send
    next_choice<uint8_t> next_protocol;
    next_choice<uint8_t> next_ttl;
    next_choice<ip4_addr> next_dest_addr;
    next_choice<ip4_addr> next_source_addr;
    msg_send_forwarder send_medium;

    int send_data(send_msg_t &&data) override;


    // recv
#define IP4_LISTEN_ON_PROTOCOL_INDEX 0
#define IP4_LISTEN_ON_SRC_ADDR_INDEX 1
#define IP4_LISTEN_ON_DEST_ADDR_INDEX 2
    multiplexer<basic_recv_listener *,
            uint8_t, // protocol, index 0
            ip4_addr, // src addr, index 1
            ip4_addr // dest addr, index 2
    > listeners;
    basic_recv_listener *default_listener = nullptr;

    void handle_callback(recv_msg_t &&data) override;

};

#endif //NETWORKING_IP4_PROTOCOL_H
