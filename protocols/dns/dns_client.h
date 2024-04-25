#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/circular_buffer.h"
using namespace std;

#define DNS_PORT 53

class dns_client : public msg_receiver {
public:

    msg_sender* gateway;

    circular_buffer<received_msg> message_q;

    void handle_received_event(received_msg msg) override;

    void query(const string &name);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
