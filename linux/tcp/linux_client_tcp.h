#ifndef SERVERCLIENT_LINUX_CLIENT_TCP_H
#define SERVERCLIENT_LINUX_CLIENT_TCP_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connection_oriented/client/basic_client.h"
#include "../linux_co_conn.h"

#include <iostream>

using namespace std;

class linux_client_tcp : public basic_client<linux_co_conn> {
protected:
    std::unique_ptr<linux_co_conn> conn_host() override;

    string ip;
    int port;

public:
    linux_client_tcp(const string &ip, int port);
};


#endif //SERVERCLIENT_LINUX_CLIENT_IP4_H
