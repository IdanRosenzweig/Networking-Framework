#ifndef SERVERCLIENT_ICMP_CONN_CLIENT_H
#define SERVERCLIENT_ICMP_CONN_CLIENT_H

#include "../../abstract/basic_encapsulating_client.h"
#include "../ip4/ip4_conn_client.h"

class icmp_conn_client
//        : public basic_encapsulating_client
        {
public:

    ip4_conn_client* ip_client;

    void ping();

};


#endif //SERVERCLIENT_ICMP_CONN_CLIENT_H
