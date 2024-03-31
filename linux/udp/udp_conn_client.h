#ifndef SERVERCLIENT_UDP_CONN_CLIENT_H
#define SERVERCLIENT_UDP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_encapsulating_client.h"
#include "../ip4/ip4_conn_client.h"
#include <string>

// udp doesn't inherit basic_encapsulating_client bacause it doesn't support a way to
// identify sub protocols. it just allows to send arbitrary payload
class udp_conn_client : public basic_cl_client {
    // ipv4_client
protected:
//    std::string ip;
    int my_port = 1212;
    int server_port;

//    int fd;
//    struct sockaddr_in addr;

public:
    udp_conn_client(int port);

    void init() override;

    void finish() override;

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;

    ip4_conn_client* ip_client;

    void recv_data(void* data, int count);

    void send_data(void* buff, int cnt);

};

#endif //SERVERCLIENT_UDP_CONN_CLIENT_H
