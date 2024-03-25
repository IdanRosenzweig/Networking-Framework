#ifndef SERVERCLIENT_LINUX_SERVER_UDP_H
#define SERVERCLIENT_LINUX_SERVER_UDP_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../linux_cl_conn.h"
#include <memory>

#include <arpa/inet.h>

class linux_server_udp : public basic_cl_server<linux_cl_conn> {
protected:
    std::unique_ptr<linux_cl_conn> discover_next_host() override;

    int port;
    int fd;

public:
    linux_server_udp(int port);
};

#endif //SERVERCLIENT_LINUX_SERVER_IP4_H
