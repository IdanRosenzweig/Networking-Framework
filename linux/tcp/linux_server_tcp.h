#ifndef SERVERCLIENT_LINUX_SERVER_TCP_H
#define SERVERCLIENT_LINUX_SERVER_TCP_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connection_oriented/server/basic_server.h"
#include "../linux_co_conn.h"
#include <iostream>

using namespace std;

class linux_server_tcp : public basic_server<linux_co_conn> {
protected:
    std::unique_ptr<linux_co_conn> conn_next_host() override;

    int port;
    int fd;

public:
    linux_server_tcp(int port);
};


#endif //SERVERCLIENT_LINUX_SERVER_IP4_H
