#ifndef SERVERCLIENT_LINUX_CLIENT_UDP_H
#define SERVERCLIENT_LINUX_CLIENT_UDP_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_encapsulating_protocol.h"
#include <string>

class linux_client_udp : public basic_cl_client, public basic_encapsulating_protocol {
    // ipv4_client
protected:
    std::string ip;
    int port;

    int fd;
    struct sockaddr_in addr;

public:
    linux_client_udp(const std::string &ip, int port);

    void init() override;

    void finish() override;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;


};

#endif //SERVERCLIENT_LINUX_CLIENT_UDP_H
