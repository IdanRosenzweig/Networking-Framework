#ifndef SERVERCLIENT_TCP_CONN_SERVER_H
#define SERVERCLIENT_TCP_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connection_oriented/server/basic_server.h"
#include <iostream>

using namespace std;

struct handler {
    int fd;
    struct sockaddr_in addr;

    int send_encapsulated_data(void *buff, int count) {
        return send(fd, buff, count, 0);
    }

    int recv_encapsulated_data(void *buff, int count) {
        return recv(fd, buff, count, 0);
    }
};

class tcp_conn_server : public basic_server<handler> {
public:
    // connect next host
    handler conn_next_host() override;

    int port;
    int fd;


public:
    tcp_conn_server(int port);

    void setup();
    void destroy();

};


#endif //SERVERCLIENT_IP4_CONN_SERVER_H
