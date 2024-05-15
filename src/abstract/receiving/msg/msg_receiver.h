#ifndef SERVERCLIENT_MSG_RECEIVER_H
#define SERVERCLIENT_MSG_RECEIVER_H

#include "../basic_receiver.h"
#include "../multi_receiver.h"
#include "received_msg.h"

using msg_receiver = basic_receiver<received_msg>;
using multi_msg_receiver = multi_receiver<received_msg>;

#endif //SERVERCLIENT_MSG_RECEIVER_H
