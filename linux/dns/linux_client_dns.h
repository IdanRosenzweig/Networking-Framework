#ifndef SERVERCLIENT_LINUX_CLIENT_DNS_H
#define SERVERCLIENT_LINUX_CLIENT_DNS_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../udp/linux_client_udp.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include <iostream>

using namespace std;

#define DNS_PORT 53

class linux_client_dns : public basic_cl_client {
    linux_client_udp udp_client;
public:

    linux_client_dns(const string &server_ip);

    void query(const string &name);

    void init() override;

    void finish() override;
};


#endif //SERVERCLIENT_LINUX_CLIENT_DNS_H
