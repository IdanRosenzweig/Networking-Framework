#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../udp/udp_conn_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include <iostream>

using namespace std;

#define DNS_PORT 53

class dns_client : public basic_cl_client {
public:

    udp_conn_client* udp_client;

    void query(const string &name);

    void init() override;

    void finish() override;
};


#endif //SERVERCLIENT_DNS_CLIENT_H
