#include <linux/if_ether.h>
#include <netinet/in.h>
#include "network_layer_gateway_client.h"

int network_layer_gateway_client::send_data(void *buff, int count) {
    return ether_client.send_data(buff, count);
}

