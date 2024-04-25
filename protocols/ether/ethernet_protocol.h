#ifndef SERVERCLIENT_ETHERNET_PROTOCOL_H
#define SERVERCLIENT_ETHERNET_PROTOCOL_H

#include <map>

#include "mac_addr.h"
#include "../../abstract/next_choice.h"
#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/multiplexer.h"

class ethernet_protocol : public msg_sender, public msg_receiver {
public:
    ethernet_protocol();

    // send
    next_choice<int> next_protocol;
    next_choice<mac_addr> next_dest_addr;
    next_choice<mac_addr> next_source_addr;

    msg_sender* gateway;

    int send_data(send_msg msg) override;

    // recv
    multiplexer<int, basic_receiver*> protocol_handlers;

    void handle_received_event(received_msg msg) override;

};


#endif //SERVERCLIENT_ETHERNET_PROTOCOL_H
