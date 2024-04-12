#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <linux/if.h>
#include <thread>

#include "mac_addr.h"
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/protocol_multiplexer.h"
#include "../../abstract/basic_conn.h"
#include "../../abstract/message.h"


class ethernet_conn_client : public basic_encapsulating_client<int> {
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
