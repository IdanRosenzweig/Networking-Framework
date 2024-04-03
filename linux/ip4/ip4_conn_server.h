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

#include "addit_data.h"

struct ip4_addr {
    uint32_t raw;
};

class ip4_conn_server : public basic_encapsulating_server<ip4_addr, int, prot_addit_data> {
private:
    void register_filter(int prot);

public:
    ip4_conn_server();

    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg( ip4_addr client, void* buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
