#ifndef SERVERCLIENT_UDP_CONN_SERVER_H
#define SERVERCLIENT_UDP_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include <memory>
#include <queue>
#include "../ip4/handler.h"
#include "../ip4/ip4_conn_server.h"
#include "udp_header.h"

#include <arpa/inet.h>

class udp_conn_server : public basic_cl_server {
//protected:
//    // discover next host trying to send us data
//    virtual sockaddr_in discover_next_host(); // todo in basic_cl_server with template
//
//public:
////    std::queue<sockaddr_in> clients_q;
//
//    sockaddr_in discover_next_client() {
////        sockaddr_in conn = discover_next_host();
////        clients_q.push(conn);
//        return discover_next_host();
//    }

    int server_port;

    int last_port; // last client's dest_port

public:
    udp_conn_server(int port);

    void setup() override;

    void destroy() override;


    ip4_conn_server* ip_server;

    int recv_data(void* data, int count);

    void send_data(void* buff, int cnt);
};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
