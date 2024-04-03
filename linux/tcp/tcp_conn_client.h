#ifndef SERVERCLIENT_TCP_CONN_CLIENT_H
#define SERVERCLIENT_TCP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connection_oriented/basic_co_client.h"
#include "../../abstract/basic_encapsulating_client.h"

#include <iostream>

using namespace std;

// tcp doesn't inherit basic_encapsulating_client because it doesn't support a way to
// identify sub protocols. it just allows to send arbitrary payload
class tcp_conn_client : public basic_co_client
//        , public basic_encapsulating_client
        {
protected:

    string ip;
    int port;

    int fd;
    struct sockaddr_in addr;

public:
    tcp_conn_client(const string &ip, int port);

    void conn() override;

    void disconn() override;

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;

    int recv_data(void* data, int count);

    int send_data(void* buff, int cnt);
};


#endif //SERVERCLIENT_IP4_CONN_CLIENT_H