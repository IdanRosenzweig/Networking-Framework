#ifndef SERVERCLIENT_MSG_GATEWAY_H
#define SERVERCLIENT_MSG_GATEWAY_H

#include "basic_gateway.h"
#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"

using msg_gateway = basic_gateway<send_msg<>, received_msg>;

#endif //SERVERCLIENT_MSG_GATEWAY_H
