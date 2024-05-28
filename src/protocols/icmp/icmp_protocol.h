#ifndef NETWORKING_ICMP_PROTOCOL_H
#define NETWORKING_ICMP_PROTOCOL_H

#include <map>

#include "icmp_header.h"
#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/utils/multiplexer.h"
#include "../../abstract/utils/next_choice.h"

class icmp_protocol : public msg_send_medium, public msg_recv_listener {
public:

    icmp_protocol() {
        // prevent linux from sending various response icmp packets that interfere
        system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_all > /dev/null");
        system("echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts > /dev/null");
    }

    // send
    msg_send_medium* gateway;
    next_choice<int> next_type;
    next_choice<int> next_code;
    next_choice<uint32_t> next_content;
    int send_data(send_msg<>&& msg) override;


    // recv
    map<int, vector<basic_recv_listener *>> type_handlers;
    basic_recv_listener* default_handler = nullptr;
    void handle_received_event(received_msg&& msg) override;

};


#endif //NETWORKING_ICMP_PROTOCOL_H
