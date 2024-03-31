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
public:
    ip4_conn_server();

    // linux won't allow to receive raw packets of any type IPPROTO_RAW, only to send ones.
    // you must specify beforehand the type of protocol you would encapsulate
    // in the ip packets, and can't change that type.
    // so this function registers a protocol that will be used in the future
    // (creates a new fd for the specific protocol)
    void register_handler(int prot);


    // receive the next msg of the encapsulated protocol
    int recv_prot_next_msg(int prot, void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_prot_msg(int prot, ip4_addr client, void* buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
