#ifndef SERVERCLIENT_LINUX_SERVER_IP4_H
#define SERVERCLIENT_LINUX_SERVER_IP4_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../linux_cl_conn.h"
#include "../../abstract/connectionless/connection/ip4_conn.h"
#include <memory>

#include <arpa/inet.h>

class linux_server_ip4 : public basic_cl_server<linux_cl_conn<ip4_conn>> {
protected:
    std::unique_ptr<linux_cl_conn<ip4_conn>> discover_next_host() override;

    int fd;

public:
    linux_server_ip4();
};

#endif //SERVERCLIENT_LINUX_SERVER_IP4_H