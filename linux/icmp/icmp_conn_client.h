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
    // linux doesn't allow to receive raw ip packets.
    // to receive ip packets, you must open a raw socket and specify the protocol you are using
    ip4_conn_client ip_client;

    int fd;

public:
    icmp_conn_client(const std::string &str);

    void init() override;

    void finish() override;

    void ping();

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;


};


#endif //SERVERCLIENT_ICMP_CONN_CLIENT_H
