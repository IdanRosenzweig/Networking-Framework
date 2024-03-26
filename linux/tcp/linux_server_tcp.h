#ifndef SERVERCLIENT_LINUX_SERVER_TCP_H
#define SERVERCLIENT_LINUX_SERVER_TCP_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connection_oriented/server/basic_server.h"
#include <iostream>

using namespace std;

class linux_server_tcp {
protected:
    struct handler{
        int fd;
        struct sockaddr_in addr;
    };
    // connect next host
    virtual handler conn_next_host();

public:
    std::vector<handler> clients;

    void accept_next_client() {
        handler conn = conn_next_host();
        clients.push_back(conn);
    }

    int port;
    int fd;

public:
    linux_server_tcp(int port);

    void setup();
    void destroy();

    int send_encapsulated_data(void *buff, int count, handler) ;

    int recv_encapsulated_data(void *buff, int count, handler) ;
};


#endif //SERVERCLIENT_LINUX_SERVER_IP4_H
