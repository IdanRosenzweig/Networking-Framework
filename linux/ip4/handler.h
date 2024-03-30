#ifndef SERVERCLIENT_HANDLER_H
#define SERVERCLIENT_HANDLER_H

#include <netinet/in.h>

// this class represents a higher layer protocol which is encapsulated
// by a lower layer protocol.

// when the lower layer receives a message with this encapsulated protocol,
// it calls recv_data()

class encapsulated_data_handler {
public:
    virtual void handle_received_data(void* data) = 0;
};

#endif //SERVERCLIENT_HANDLER_H
