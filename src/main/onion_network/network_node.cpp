#include "../../linux/osi/network_layer_gateway.h"
#include "../../temp_utils/onion_network/onion_network_node.h"


void onion_network_node_main() {
    network_layer_gateway ipNetworkGateway("enp0s3"); // gateway to ip network
    onion_network_node node(&ipNetworkGateway);

    while (true) {

    }
}

int main() {
    onion_network_node_main();
}