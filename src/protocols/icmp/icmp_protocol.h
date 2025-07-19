#ifndef NETWORKING_ICMP_PROTOCOL_H
#define NETWORKING_ICMP_PROTOCOL_H

#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../utils/multiplexer.h"
#include "../../utils/next_choice.h"
#include "icmp_header.h"

class icmp_protocol : public msg_send_medium, public msg_recv_listener {
public:

    icmp_protocol();

    // send
    next_choice<int> next_type;
    next_choice<int> next_code;
    next_choice<uint32_t> next_content;
    msg_send_forwarder send_medium;

    int send_data(send_msg_t &&data) override;


    // recv
#define ICMP_LISTEN_ON_TYPE_INDEX 0
#define ICMP_LISTEN_ON_CODE_INDEX 1
    multiplexer<basic_recv_listener *,
            uint8_t, // type, index 0
            uint8_t // code, index 1
    > listeners;
    basic_recv_listener *default_listener = nullptr;

    void handle_callback(recv_msg_t &&data) override;

};


#endif //NETWORKING_ICMP_PROTOCOL_H
