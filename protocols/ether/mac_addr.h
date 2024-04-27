#ifndef SERVERCLIENT_MAC_ADDR_H
#define SERVERCLIENT_MAC_ADDR_H

#include <stdint.h>
#include "../ip4/ip4_addr.h"

struct mac_addr {
    uint8_t octets[6];
};

#define BROADCAST_MAC (mac_addr{0xff,0xff,0xff,0xff,0xff,0xff})

//mac_addr get_my_mac_address(const char *interface_name);
//
//void print_mac(mac_addr octets);
//
//ip4_addr get_my_priv_ip_addr(const char *interface);

#endif //SERVERCLIENT_MAC_ADDR_H
