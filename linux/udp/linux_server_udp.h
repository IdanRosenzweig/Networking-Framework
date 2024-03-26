#ifndef SERVERCLIENT_LINUX_SERVER_UDP_H
#define SERVERCLIENT_LINUX_SERVER_UDP_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../linux_cl_conn.h"
#include <memory>

#include <arpa/inet.h>

class linux_server_udp  {
protected:
    // discover next host trying to send us data
    virtual sockaddr_in discover_next_host();

public:
    std::queue<sockaddr_in> clients_q;

    void discover_next_client() {
        sockaddr_in conn = discover_next_host();
        clients_q.push(conn);
    }

    int port;
    int fd;

public:
    linux_server_udp(int port);

    void setup();
    void destroy();

    int send_encapsulated_data(void *buff, int count, sockaddr_in addr) ;

    int recv_encapsulated_data(void *buff, int count, sockaddr_in addr) ;
};

#endif //SERVERCLIENT_LINUX_SERVER_IP4_H
