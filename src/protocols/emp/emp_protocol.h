#ifndef NETWORKING_EMP_PROTOCOL_H
#define NETWORKING_EMP_PROTOCOL_H

#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/utils/next_choice.h"
#include "../../abstract/utils/udata.h"
#include "../../abstract/utils/trie.h"

// endpoint multiplexing protocol
class emp_protocol : public msg_send_medium, public msg_recv_listener {
public:

    // send
    next_choice<udata_t> next_source_point;
    next_choice<udata_t> next_dest_point;
    msg_send_forwarder send_medium;

    int send_data(send_msg_t&&data) override;

    // recv
    trie_node<uint8_t, 256, [](uint8_t val) -> size_t {return val;}, vector<basic_recv_listener*>> endpoints_handlers;
    basic_recv_listener* default_handler = nullptr;

    void handle_callback(recv_msg_t&& data) override;

};

#endif //NETWORKING_EMP_PROTOCOL_H
