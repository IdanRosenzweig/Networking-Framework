#ifndef SERVERCLIENT_IP4_CONN_CLIENT_H
#define SERVERCLIENT_IP4_CONN_CLIENT_H

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../abstract/basic_encapsulating_client.h"

#include "../ether/ethernet_conn_client.h"


class ip4_conn_client : public basic_encapsulating_client<int> {
protected:
    std::string ip;
    struct sockaddr_in dest_addr;

    int fd;

    protocol_multiplexer<int, circular_buffer<message, MAX_NO_MSG>> protocolQueue;

public:

    ip4_conn_client(const std::string &str);

    ethernet_conn_client *ether_client;

    // receive the next msg of the encapsulated protocol
    int recv_next_msg(void *buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg(void *buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_CLIENT_H
