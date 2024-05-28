#ifndef SERVERCLIENT_MSG_RECEIVER_H
#define SERVERCLIENT_MSG_RECEIVER_H

#include "../basic_receiver.h"
#include "../receive_forwarder.h"
#include "received_msg.h"

using msg_receiver = basic_receiver<received_msg>;
using msg_receive_multiplexer = receive_forwarder<received_msg>;

#endif //SERVERCLIENT_MSG_RECEIVER_H
