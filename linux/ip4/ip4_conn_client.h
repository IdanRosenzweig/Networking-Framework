#ifndef SERVERCLIENT_IP4_CONN_CLIENT_H
#define SERVERCLIENT_IP4_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_protocol.h"
#include "../../abstract/connectionless/basic_cl_client.h"

class ip4_conn_client : public basic_cl_client, public basic_encapsulating_protocol {
//    network_layer_gateway
protected:
    int fd;
    std::string ip;
    struct sockaddr_in addr;
public:
    ip4_conn_client(const std::string &ip);

    void init() override;

    void finish() override;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;
};

#endif //SERVERCLIENT_IP4_CONN_CLIENT_H
