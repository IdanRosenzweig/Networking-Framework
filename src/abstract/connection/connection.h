#ifndef SERVERCLIENT_CONNECTION_H
#define SERVERCLIENT_CONNECTION_H

#include "../sending/basic_send_medium.h"
#include "../sending/msg/send_msg.h"

#include "../receiving/recv_forwarder.h"
#include "../receiving/msg/received_msg.h"

// a basic connection that you can send data on and listen to data from

class connection : public basic_send_medium<send_msg<>>, public recv_forwarder<received_msg> {
};


#endif //SERVERCLIENT_CONNECTION_H
