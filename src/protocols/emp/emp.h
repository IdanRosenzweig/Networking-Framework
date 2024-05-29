#ifndef NETWORKING_EMP_H
#define NETWORKING_EMP_H

#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/udata.h"
#include "../../abstract/utils/trie.h"

// endpoint multiplexing protocol
class emp : public msg_send_medium, public msg_recv_listener {
public:

    // send
    next_choice<udata_t> next_source_point;
    next_choice<udata_t> next_dest_point;
    msg_send_forwarder send_medium;

    int send_data(send_msg<>&& msg) override;

    // recv
    trie_node<uint8_t, 256, [](uint8_t val) -> size_t {return val;}, vector<basic_recv_listener*>> endpoints_handlers;
    basic_recv_listener* default_handler = nullptr;

    void handle_received_event(received_msg&& msg) override;

};

#endif //NETWORKING_EMP_H
