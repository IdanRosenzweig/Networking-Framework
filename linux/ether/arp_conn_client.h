#ifndef SERVERCLIENT_ARP_CONN_CLIENT_H
#define SERVERCLIENT_ARP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_protocol.h"
#include "../../abstract/connectionless/basic_cl_client.h"

class arp_conn_client : public basic_cl_client {
protected:
    int fd;

    std::string mac;
//    struct sockaddr_in dest_addr;

public:
    arp_conn_client(const std::string& str);

    void init() override;

    void finish() override;

    void discover(const std::string ip);
};

#endif //SERVERCLIENT_ARP_CONN_CLIENT_H
