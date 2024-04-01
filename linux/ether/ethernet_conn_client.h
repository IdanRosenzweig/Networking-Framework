#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include "../ip4/addit_data.h"

class ethernet_conn_client : public basic_cl_client, public basic_encapsulating_client<int, prot_addit_data> {
//    data_link_layer_gateway
protected:

    std::string mac;
    struct sockaddr_ll dest_addr;

    struct ifreq if_idx;
    struct ifreq my_mac;
    struct ifreq my_priv_ip;

public:
    ethernet_conn_client(const std::string& mac);

    void init() override;

    void finish() override;
//
//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;

    // linux won't allow to receive raw packets of any type, only to send ones.
    // you must specify beforehand the type of protocol you would encapsulate
    // in the ip packets, and can't change that type.
    // so this function registers a protocol that will be used in the future
    // (creates a new fd for the specific protocol)
    void register_handler(int prot);


    // receive the next msg of the encapsulated protocol
    int recv_prot_next_msg(int prot, void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_prot_msg(int prot, void* buff, int count) override;

    void spoof();
};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
