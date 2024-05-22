#ifndef SERVERCLIENT_MSG_SNIFFER_H
#define SERVERCLIENT_MSG_SNIFFER_H

#include "basic_sniffer.h"
#include "sniff_multiplexer.h"
//#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"

using msg_sniffer = basic_sniffer<received_msg, received_msg>;
using multi_msg_sniffer = sniff_multiplexer<received_msg, received_msg>;

#endif //SERVERCLIENT_MSG_SNIFFER_H
