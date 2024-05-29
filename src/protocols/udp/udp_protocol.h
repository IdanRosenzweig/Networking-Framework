#ifndef NETWORKING_UDP_PROTOCOL_H
#define NETWORKING_UDP_PROTOCOL_H

#include <map>

#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"
#include "udp_header.h"

class udp_protocol : public msg_send_medium, public msg_recv_listener {
public:

    udp_protocol() {
        system("sudo iptables -A OUTPUT -p icmp --icmp-type destination-unreachable -j DROP > /dev/null");

    }

    // send
    next_choice<uint16_t> next_source_port;
    next_choice<uint16_t> next_dest_port;
    msg_send_forwarder send_medium;

    int send_data(send_msg<> &&msg) override;


    // recv
#define UDP_LISTEN_ON_PORT_INDEX 0
    multiplexer<basic_recv_listener *,
            uint16_t // port, index 0
    > listeners;
    basic_recv_listener *default_listener = nullptr;

    void handle_received_event(received_msg &&msg) override;

};

#endif //NETWORKING_UDP_PROTOCOL_H
