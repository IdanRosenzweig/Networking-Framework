#ifndef SERVERCLIENT_BASIC_CONNECTION_H
#define SERVERCLIENT_BASIC_CONNECTION_H

#include "../sending/msg_sender.h"
#include "../receiving/listenable.h"
#include "../receiving/received_msg.h"

// a basic connection that you can send data on and listen to data from
class basic_connection : public msg_sender, public listenable<received_msg> {
public:
    virtual ~basic_connection() {}
};

#endif //SERVERCLIENT_BASIC_CONNECTION_H
