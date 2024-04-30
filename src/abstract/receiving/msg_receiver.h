#ifndef SERVERCLIENT_MSG_RECEIVER_H
#define SERVERCLIENT_MSG_RECEIVER_H

#include "basic_receiver.h"
#include "received_msg.h"

class msg_receiver : public basic_receiver<received_msg> {
};

#endif //SERVERCLIENT_MSG_RECEIVER_H
