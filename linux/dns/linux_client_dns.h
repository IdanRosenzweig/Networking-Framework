#ifndef SERVERCLIENT_LINUX_CLIENT_DNS_H
#define SERVERCLIENT_LINUX_CLIENT_DNS_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../udp/linux_client_udp.h"

#include <iostream>

using namespace std;

#define DNS_PORT 53

class linux_client_dns : public linux_client_udp {
public:
    linux_client_dns(const string &server_ip);

    void query(const string &name);
};


#endif //SERVERCLIENT_LINUX_CLIENT_DNS_H
