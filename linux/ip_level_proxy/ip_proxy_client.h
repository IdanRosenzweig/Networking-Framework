#ifndef SERVERCLIENT_IP_PROXY_CLIENT_H
#define SERVERCLIENT_IP_PROXY_CLIENT_H


#include "../network_layer_gateway/network_layer_gateway_client.h"
#include "../../protocols/ip4/ip4_conn_client.h"
#include "../../protocols/udp/udp_conn_client.h"

// gateway at the ip level (changes ip header)
class ip_proxy_client : public basic_gateway {
    // connection to gateway server using udp
    network_layer_gateway_client _networkLayerGateway; // instead of providing the router's mac address, this calls wraps it for us

    ip4_conn_client _ip_client;
    udp_conn_client _udp_client;

public:
    ip_proxy_client() {
        // setup send flow
        _ip_client.gateway = &_networkLayerGateway;
        _ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
        _ip_client.next_server_addr.set_next_choice(convert_to_ip4_addr("10.100.102.18"));

        _udp_client.gateway = &_ip_client;
        _udp_client.my_port.set_next_choice(1212);
        _udp_client.next_external_port.set_next_choice(4444);

        // setup recv flow
        _networkLayerGateway.add_listener(&_ip_client);

        _ip_client.prot_handlers[IPPROTO_UDP] = &_udp_client;

        _udp_client.port_handlers[1212] = this;
    }

    int send_data(void *buff, int count) override {
        return _udp_client.send_data(buff, count);
    }

};


#endif //SERVERCLIENT_IP_PROXY_CLIENT_H
