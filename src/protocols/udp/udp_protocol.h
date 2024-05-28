#ifndef SERVERCLIENT_UDP_PROTOCOL_H
#define SERVERCLIENT_UDP_PROTOCOL_H

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
    next_choice<int> next_source_port;
    next_choice<int> next_dest_port;
    msg_send_medium* send_medium;

    int send_data(send_msg<>&& msg) override;


    // recv
    map<int, vector<basic_recv_listener *>> port_handlers;
    basic_recv_listener* default_handler = nullptr;
    void handle_received_event(received_msg&& msg) override;

};

#endif //SERVERCLIENT_UDP_PROTOCOL_H
