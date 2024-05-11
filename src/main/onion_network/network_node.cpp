#include "../../proxy/network_layer/ip_proxy_server.h"
#include "../../temp_connections/udp_client_server/udp_server.h"
#include "../../onion_network/onion_network_node.h"


void onion_network_node_main() {
    network_layer_gateway ipNetworkGateway("enp0s3"); // gateway to ip network
    onion_network_node node(&ipNetworkGateway);

    while (true) {

    }
}

int main() {
    onion_network_node_main();
}