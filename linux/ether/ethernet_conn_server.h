#ifndef SERVERCLIENT_ETHERNET_CONN_SERVER_H
#define SERVERCLIENT_ETHERNET_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <queue>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include "../ip4/addit_data.h"
#include "mac_addr.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../../abstract/protocol_queue.h"
#include "../../abstract/connectionless/basic_cl_server.h"

class ethernet_conn_server : public basic_cl_server {
private:
    data_link_layer_gateway gateway;

    std::thread worker;

    protocol_queue<int> protocolQueue;

public:

    ethernet_conn_server();

    virtual ~ethernet_conn_server();

    void setup() override;

    void destroy() override;

private:
    int next_prot; // indicated the protocol used in the next encapsulated message
public:
    int getNextProt() const {return next_prot;}

    void setNextProt(int nextProt) {next_prot = nextProt;}


    mac_addr my_mac;
    struct ifreq my_priv_ip;

    mac_addr dest_device = BROADCAST_MAC;
    void change_dest_mac(mac_addr mac);


    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count);

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count);

};


#endif //SERVERCLIENT_ETHERNET_CONN_SERVER_H
