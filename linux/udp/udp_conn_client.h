#ifndef SERVERCLIENT_UDP_CONN_CLIENT_H
#define SERVERCLIENT_UDP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_encapsulating_client.h"
#include <string>

class udp_conn_client : public basic_cl_client, public basic_encapsulating_client {
    // ipv4_client
protected:
    std::string ip;
    int port;

    int fd;
    struct sockaddr_in addr;

public:
    udp_conn_client(const std::string &ip, int port);

    void init() override;

    void finish() override;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;


};

#endif //SERVERCLIENT_UDP_CONN_CLIENT_H
