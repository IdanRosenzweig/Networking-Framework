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
    struct sockaddr_in dest_addr;

    int prot;

public:
    // linux won't allow to receive raw packets of any type IPPROTO_RAW, only to send ones.
    // you must specify beforehand the type of protocol you would encapsulate
    // in the ip packets, and can't change that type
    ip4_conn_client(const std::string& str, int protocol);

    void init() override;

    void finish() override;

    int send_encapsulated_data(void *buff, int count) override;

    int recv_encapsulated_data(void *buff, int count) override;
};

#endif //SERVERCLIENT_IP4_CONN_CLIENT_H
