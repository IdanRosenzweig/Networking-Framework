#ifndef SERVERCLIENT_ICMP_CONN_CLIENT_H
#define SERVERCLIENT_ICMP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_encapsulating_client.h"
#include "../ip4/ip4_conn_client.h"
class icmp_conn_client : public basic_cl_client
//        , public basic_encapsulating_client
        {

protected:
public:

    ip4_conn_client* ip_client;

    void init() override;

    void finish() override;

    void ping();

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;


};


#endif //SERVERCLIENT_ICMP_CONN_CLIENT_H
