#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

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



class ethernet_conn_client : public basic_cl_client, public basic_encapsulating_client<int, prot_addit_data> {
private:
    data_link_layer_gateway gateway;

    std::thread worker;

    protocol_queue<int> protocolQueue;

    mac_addr dest_device = BROADCAST_MAC;

public:

    mac_addr my_mac;
    struct ifreq my_priv_ip;

    void change_dest_mac(mac_addr mac);

    ethernet_conn_client();

    virtual ~ethernet_conn_client();

    void init() override;

    void finish() override;


    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count) override;

};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
