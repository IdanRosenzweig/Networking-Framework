#ifndef SERVERCLIENT_EMP_H
#define SERVERCLIENT_EMP_H

#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/udata.h"
#include "../../abstract/utils/trie.h"

// endpoint multiplexing protocol
class emp : public msg_sender, public msg_receiver {
public:

    // send
    next_choice<udata_t> next_source_point;
    next_choice<udata_t> next_dest_point;
    msg_sender* gateway;

    int send_data(send_msg<>& msg) override;

    // recv
    trie_node<uint8_t, 256, [](uint8_t val) -> size_t {return val;}, basic_receiver*> endpoints_handlers;
    basic_receiver* default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;

};

#endif //SERVERCLIENT_IP4_PROTOCOL_H
