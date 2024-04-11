#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <queue>
#include "../../abstract/basic_encapsulating_client.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include "../ip4/addit_data.h"
#include "mac_addr.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../../abstract/protocol_multiplexer.h"



class ethernet_conn_client : public basic_encapsulating_client<int, prot_addit_data> {
public:
//    data_link_layer_gateway gateway;
    basic_conn* gateway = nullptr;

    std::thread worker;

    mac_addr dest_device = BROADCAST_MAC;

public:

    mac_addr my_mac;
    struct ifreq my_priv_ip;

    void change_dest_mac(mac_addr mac);

//    pcap_t *handle; // pcap handle for capturing packets
    ethernet_conn_client();

    virtual ~ethernet_conn_client();


    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count) override;

    protocol_multiplexer<int, circular_buffer<message, MAX_NO_MSG>> protocolQueue;
};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
