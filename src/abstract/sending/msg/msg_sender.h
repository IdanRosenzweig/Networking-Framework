#ifndef SERVERCLIENT_MSG_SENDER_H
#define SERVERCLIENT_MSG_SENDER_H

#include "../basic_sender.h"
#include "../multi_sender.h"
#include "send_msg.h"

typedef basic_sender<send_msg> msg_sender;
typedef multi_sender<send_msg> multi_msg_sender;

#endif //SERVERCLIENT_MSG_SENDER_H
