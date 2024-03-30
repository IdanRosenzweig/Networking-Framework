#ifndef SERVERCLIENT_IP4_CONN_SERVER_H
#define SERVERCLIENT_IP4_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../linux_cl_conn.h"
#include "../udp/udp_conn_client.h"
#include "../udp/udp_conn_server.h"
#include <memory>
#include "handler.h"

#include <arpa/inet.h>
#include <map>

struct ip4_handler {
    int fd;
    struct sockaddr_in addr;

    int send_encapsulated_data(void *buff, int count) {
        return send(fd, buff, count, 0);
    }

    int recv_encapsulated_data(void *buff, int count) {
        return recv(fd, buff, count, 0);
    }
};

class ip4_conn_server {
protected:
    // discover next host trying to send us data
    virtual sockaddr_in discover_next_host();

public:
//    std::queue<sockaddr_in> clients_q;

    sockaddr_in discover_next_client() {
//        sockaddr_in conn = discover_next_host();
//        clients_q.push(conn);
        return discover_next_host();
    }

public:
    ip4_conn_server();

    struct prot_st {
        int fd;
        class encapsulated_data_handler* handler;
    };
    std::map<int, prot_st> prot_handlers;

    void register_handler(int prot, class encapsulated_data_handler* handler);

    void handle_prot_next_msg(int prot);
};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
