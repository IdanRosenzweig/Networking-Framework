#ifndef SERVERCLIENT_IP4_CONN_SERVER_H
#define SERVERCLIENT_IP4_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../../abstract/basic_encapsulating_server.h"
#include <memory>
#include "handler.h"

#include <arpa/inet.h>
#include <map>
#include <queue>
#include <string>

struct ip4_addr {
    uint32_t raw;
};

struct port_data {
    // the only thing we need to store is the fd of the opened linux socket
    int fd{};

};

class ip4_conn_server : public basic_encapsulating_server<ip4_addr, int, port_data> {
public:
    ip4_conn_server();

    // register a handler for receiving an encapsulated protocol.
    //
    void register_handler(int prot) override;


    // receive the next msg of the encapsulated protocol
    void recv_prot_next_msg(int prot) override;

    // send message to the last client that sent message with the protocol
    void send_next_prot_msg(int prot, ip4_addr client, void* buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
