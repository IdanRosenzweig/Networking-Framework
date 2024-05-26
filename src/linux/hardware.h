#ifndef SERVERCLIENT_HARDWARE_H
#define SERVERCLIENT_HARDWARE_H

#include "../protocols/ether/mac_addr.h"
#include "../protocols/ip4/ip4_addr.h"

mac_addr get_mac_addr_of_iface(const string &iface);

void set_mac_addr_for_iface(const string &iface, mac_addr new_addr);

ip4_addr get_ip_addr_of_iface(const string &iface);

ip4_addr get_default_gateway_of_iface(const string &iface);

#endif //SERVERCLIENT_HARDWARE_H
