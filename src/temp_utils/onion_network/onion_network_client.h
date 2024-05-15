#ifndef SERVERCLIENT_ONION_NETWORK_CLIENT_H
#define SERVERCLIENT_ONION_NETWORK_CLIENT_H

#include "../proxy/ip_proxy_client.h"
#include "../../temp_connections/udp_client_server/udp_client.h"
#include "onion_network_common.h"
#include <vector>

class onion_network_client : public msg_gateway {
public:
    struct chain_node {
        std::unique_ptr<udp_client> udp = nullptr;
        std::unique_ptr<ip_proxy_client> proxy = nullptr;
    };
    std::vector<chain_node> proxies_chain;

    onion_network_client(const vector<ip4_addr>& chain) {
        int len = chain.size();
        for (int i = 0; i < len; i++) {
            chain_node node;

            msg_gateway* gw = (i > 0) ? (msg_gateway*) proxies_chain.back().proxy.get() : nullptr;
            node.udp = std::make_unique<udp_client>(chain[i], ONION_NETWORK_NODE_LISTEN_PORT, 2001, gw);

            node.proxy = std::make_unique<ip_proxy_client>(node.udp.get());

            proxies_chain.push_back(std::move(node));
        }

        ((msg_gateway*) proxies_chain.back().proxy.get())->add_listener(this);
    }

    int send_data(send_msg& val) override {
        return proxies_chain.back().proxy->send_data(val);
    }

    void handle_received_event(received_msg &event) override {
        multi_receiver::handle_received_event(event);
    }

};


#endif //SERVERCLIENT_ONION_NETWORK_CLIENT_H
