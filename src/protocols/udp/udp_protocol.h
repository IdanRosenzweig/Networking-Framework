#ifndef SERVERCLIENT_UDP_PROTOCOL_H
#define SERVERCLIENT_UDP_PROTOCOL_H

#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"
#include "udp_header.h"

class udp_protocol : public msg_sender, public msg_receiver {
public:

    udp_protocol() {
        // prevent linux from sending "destination port unreachable" when this class receives packets.
        // because this class doesn't bind any port in the kernel
        system("sudo iptables -A OUTPUT -p icmp --icmp-type destination-unreachable -j DROP");
    }

    // send
    next_choice<int> next_source_port;
    next_choice<int> next_dest_port;

    msg_sender* gateway;

    int send_data(send_msg& msg) override;

    // recv
    multiplexer<int, basic_receiver*> port_handlers;
    basic_receiver* default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;

};

#endif //SERVERCLIENT_IP4_PROTOCOL_H
