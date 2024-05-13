#ifndef SERVERCLIENT_HARDWARE_H
#define SERVERCLIENT_HARDWARE_H

#include "../protocols/ether/mac_addr.h"
#include "../protocols/ip4/ip4_addr.h"

mac_addr get_my_mac_address(const char *interface_name);

ip4_addr get_my_priv_ip_addr(const char *interface);


#endif //SERVERCLIENT_HARDWARE_H
