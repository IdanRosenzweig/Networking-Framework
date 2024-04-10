#ifndef SERVERCLIENT_UDP_CONN_CLIENT_H
#define SERVERCLIENT_UDP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/basic_encapsulating_client.h"
#include "../ip4/ip4_conn_client.h"
#include <string>

// udp doesn't inherit basic_encapsulating_client bacause it doesn't support a way to
// identify sub protocols. it just allows to send arbitrary payload
class udp_conn_client {
    // ipv4_client
protected:
//    std::string ip;
    int my_port = 1212;
    int dest_port;

//    int fd;
//    struct sockaddr_in addr;

public:
    udp_conn_client(int dest_port, int my_port);

    ip4_conn_client* ip_client;

    int recv_data(void* data, int count);

    int send_data(void* buff, int cnt);

};

#endif //SERVERCLIENT_UDP_CONN_CLIENT_H
