#ifndef SERVERCLIENT_PACKET_DISPLAYER_H
#define SERVERCLIENT_PACKET_DISPLAYER_H

#include "../../abstract/receiving/msg_receiver.h"

class packet_displayer : public msg_receiver {
public:
    void handle_received_event(received_msg& event) override;
};


#endif //SERVERCLIENT_PACKET_DISPLAYER_H
