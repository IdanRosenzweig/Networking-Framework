#ifndef SERVERCLIENT_BASIC_GATEWAY_H
#define SERVERCLIENT_BASIC_GATEWAY_H

#include "sending/msg_sender.h"
#include "receiving/listenable.h"
#include "receiving/received_msg.h"

class basic_gateway : public msg_sender, public listenable<received_msg> {
};

#endif //SERVERCLIENT_BASIC_GATEWAY_H
