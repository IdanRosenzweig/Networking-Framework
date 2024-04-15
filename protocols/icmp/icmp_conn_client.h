#ifndef SERVERCLIENT_ICMP_CONN_CLIENT_H
#define SERVERCLIENT_ICMP_CONN_CLIENT_H


#include "../../abstract/basic_send_conn.h"
#include "../../abstract/basic_recv_conn.h"

class icmp_conn_client : public basic_recv_conn {
public:
    basic_send_conn* gateway;

    void ping();

};


#endif //SERVERCLIENT_ICMP_CONN_CLIENT_H
