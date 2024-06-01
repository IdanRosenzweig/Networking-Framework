#ifndef NETWORKING_CONNECTION_H
#define NETWORKING_CONNECTION_H

#include "../sending/basic_send_medium.h"
#include "../sending/msg/send_msg_t.h"

#include "../receiving/recv_forwarder.h"
#include "../receiving/msg/recv_msg_t.h"

// a basic connection that you can send buff on and listen to buff from
class connection : public basic_send_medium<send_msg_t>, public recv_forwarder<recv_msg_t> {
};


#endif //NETWORKING_CONNECTION_H
