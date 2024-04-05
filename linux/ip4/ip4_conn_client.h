#ifndef SERVERCLIENT_IP4_CONN_CLIENT_H
#define SERVERCLIENT_IP4_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include "addit_data.h"
#include "../ether/ethernet_conn_client.h"

class ip4_conn_client : public basic_cl_client, public basic_encapsulating_client<int, prot_addit_data> {
//    network_layer_gateway
private:
// instead of listening and filtering encapsulated protocols ourselves,
// linux will do it for us. just create a file descriptor for the dedicated protocol
// (regardless, linux doesn't provide an easy way to listen to packets with arbitrary encapsulated protocol.
// you can only send packets with arbitrary protocol using IPPROTO_RAW, but not receive ones).
//    void register_filter(int prot);

protected:
    std::string ip;
    struct sockaddr_in dest_addr;

    int fd;


    protocol_queue<int> protocolQueue;

public:

    ethernet_conn_client* ether_client;

    // linux won't allow to receive raw packets of any type IPPROTO_RAW, only to send ones.
    // you must specify beforehand the type of protocol you would encapsulate
    // in the ip packets, and can't change that type
    ip4_conn_client(const std::string &str);

    void init() override;

    void finish() override;

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;


    // receive the next msg of the encapsulated protocol
    int recv_next_msg(void *buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg(void *buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_CLIENT_H
