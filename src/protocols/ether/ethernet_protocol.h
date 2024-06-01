#ifndef NETWORKING_ETHERNET_PROTOCOL_H
#define NETWORKING_ETHERNET_PROTOCOL_H

#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/multiplexer.h"
#include "ethernet_header.h"

class ethernet_protocol : public msg_send_medium, public msg_recv_listener {
public:
    ethernet_protocol();

    // send
    next_choice<int> next_protocol;
    next_choice<mac_addr> next_dest_addr;
    next_choice<mac_addr> next_source_addr;

    msg_send_forwarder send_medium;

    int send_data(send_msg_t &&data) override;


    // recv
#define ETHER_LISTEN_ON_PROTOCOL_INDEX 0
#define ETHER_LISTEN_ON_DEST_ADDR_INDEX 1
#define ETHER_LISTEN_ON_SRC_ADDR_INDEX 2
    multiplexer<basic_recv_listener *,
            uint16_t, // protocol, index 0
            mac_addr, // dest mac addr, index 1
            mac_addr // source mac addr, index 2
    > listeners;
    basic_recv_listener *default_listener = nullptr;

    void handle_callback(recv_msg_t &&data) override;

};


#endif //NETWORKING_ETHERNET_PROTOCOL_H
