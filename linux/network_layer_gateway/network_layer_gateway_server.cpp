#include <linux/if_ether.h>
#include <netinet/in.h>
#include "network_layer_gateway_server.h"

int network_layer_gateway_server::send_data(void *buff, int count) {
    return ether_server.send_data(buff, count);
}
