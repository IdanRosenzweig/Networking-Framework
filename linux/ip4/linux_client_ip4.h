#ifndef SERVERCLIENT_LINUX_CLIENT_IP4_H
#define SERVERCLIENT_LINUX_CLIENT_IP4_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_client.h"
#include "../../abstract/basic_encapsulating_protocol.h"
#include "../../abstract/connection_oriented/client/basic_client.h"

class linux_client_ip4 : public basic_encapsulating_protocol {
//    network_layer_gateway
protected:
    int fd;
    std::string ip;
    struct sockaddr_in addr;
public:
    linux_client_ip4(const std::string &ip);

    void init() ;

    void finish() ;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;
};

#endif //SERVERCLIENT_LINUX_CLIENT_IP4_H
