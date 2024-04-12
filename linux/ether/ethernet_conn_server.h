#ifndef SERVERCLIENT_ETHERNET_CONN_SERVER_H
#define SERVERCLIENT_ETHERNET_CONN_SERVER_H

#include <linux/if.h>

#include "mac_addr.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../../abstract/basic_encapsulating_server.h"
#include "../../abstract/protocol_multiplexer.h"
#include "../../abstract/message.h"

class ethernet_conn_server : public basic_encapsulating_server<int, mac_addr> {
private:
    data_link_layer_gateway gateway;

    std::thread worker;

    protocol_multiplexer<int, circular_buffer<message, MAX_NO_MSG>> protocolQueue;

public:

    ethernet_conn_server();

    virtual ~ethernet_conn_server();

    mac_addr my_mac;
    struct ifreq my_priv_ip;

    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count);

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count);

};


#endif //SERVERCLIENT_ETHERNET_CONN_SERVER_H
