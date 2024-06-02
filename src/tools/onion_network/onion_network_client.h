#ifndef NETWORKING_ONION_NETWORK_CLIENT_H
#define NETWORKING_ONION_NETWORK_CLIENT_H

#include "../proxy/ip_proxy_client.h"
#include "../../direct_connections/udp/udp_client.h"
#include "onion_network_common.h"

#include <vector>
#include <memory>

class onion_network_client : public gateway {
public:
    struct chain_node {
        std::unique_ptr<udp_client> udp = nullptr;
        std::unique_ptr<ip_proxy_client> proxy = nullptr;
    };
    std::vector<chain_node> proxies_chain;

    onion_network_client(const vector<ip4_addr>& chain, ip4_addr src_ip, gateway* network_layer_gw);

    int send_data(send_msg_t&& data) override;

    void handle_callback(recv_msg_t &&data) override;

};


#endif //NETWORKING_ONION_NETWORK_CLIENT_H
