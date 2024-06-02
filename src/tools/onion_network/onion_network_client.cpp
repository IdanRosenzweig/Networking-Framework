#include "onion_network_client.h"

onion_network_client::onion_network_client(const vector<ip4_addr> &chain, ip4_addr src_ip, gateway *network_layer_gw) {
    int len = chain.size();
    for (int i = 0; i < len; i++) {
        chain_node node;

        gateway* gw = (i > 0) ? (gateway*) proxies_chain.back().proxy.get() : network_layer_gw;
        node.udp = std::make_unique<udp_client>(chain[i], ONION_NETWORK_NODE_LISTEN_PORT, 2001, src_ip, gw);

        node.proxy = std::make_unique<ip_proxy_client>(node.udp.get());

        proxies_chain.push_back(std::move(node));
    }

    ((gateway*) proxies_chain.back().proxy.get())->add_listener(this);
}

int onion_network_client::send_data(send_msg_t &&data) {
    return proxies_chain.back().proxy->send_data(std::move(data));
}

void onion_network_client::handle_callback(recv_msg_t &&data) {
    recv_forwarder::handle_callback(std::move(data));
}
