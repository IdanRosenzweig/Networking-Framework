#ifndef SERVERCLIENT_LINUX_CLIENT_TCP_H
#define SERVERCLIENT_LINUX_CLIENT_TCP_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connection_oriented/client/basic_client.h"
#include "../../abstract/basic_encapsulating_protocol.h"

#include <iostream>

using namespace std;

class linux_client_tcp : public basic_encapsulating_protocol {
protected:

    string ip;
    int port;

    int fd;
    struct sockaddr_in addr;

public:
    linux_client_tcp(const string &ip, int port);

    void conn() ;
    void disconn() ;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;
};


#endif //SERVERCLIENT_LINUX_CLIENT_IP4_H
