#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/received_msg.h"
#include "../../abstract/sending/msg_sender.h"

#include "dns.h"

class dns_client : public recv_queue<received_msg> {
public:

    msg_sender* gateway;

    void query(const std::string& names);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
