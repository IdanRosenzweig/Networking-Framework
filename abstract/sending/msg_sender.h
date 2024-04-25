#ifndef SERVERCLIENT_MSG_SENDER_H
#define SERVERCLIENT_MSG_SENDER_H

#include "basic_sender.h"
#include "send_msg.h"

// a basic network gateway to send data
typedef basic_sender<send_msg> msg_sender;

#endif //SERVERCLIENT_MSG_SENDER_H
