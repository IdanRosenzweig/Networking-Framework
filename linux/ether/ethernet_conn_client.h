#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

class ethernet_conn_client : public basic_cl_client, public basic_encapsulating_protocol {
//    data_link_layer_gateway
protected:
    int fd;

    std::string mac;
//    struct sockaddr_in dest_addr;

    int prot;

public:
    ethernet_conn_client(const std::string& str, int protocol);

    void init() override;

    void finish() override;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;
};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
