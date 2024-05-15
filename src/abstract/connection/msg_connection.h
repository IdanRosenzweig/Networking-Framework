#ifndef SERVERCLIENT_MSG_CONNECTION_H
#define SERVERCLIENT_MSG_CONNECTION_H

#include "basic_connection.h"
#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"

using msg_connection = basic_connection<send_msg, received_msg>;

#endif //SERVERCLIENT_MSG_CONNECTION_H
