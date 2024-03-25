#ifndef SERVERCLIENT_LINUX_CLIENT_IP4_H
#define SERVERCLIENT_LINUX_CLIENT_IP4_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../linux_cl_conn.h"
#include <string>

class linux_client_ip4 : public basic_cl_client<linux_cl_conn> {
protected:
    std::unique_ptr<linux_cl_conn> conn_to_host() override;

    std::string ip;

public:
    linux_client_ip4(const std::string &ip);
};

#endif //SERVERCLIENT_LINUX_CLIENT_IP4_H
