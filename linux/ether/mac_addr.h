#ifndef SERVERCLIENT_MAC_ADDR_H
#define SERVERCLIENT_MAC_ADDR_H

#include <stdint.h>

struct mac_addr {
    uint8_t addr[6];
};

#define BROADCAST_MAC (mac_addr{0xff,0xff,0xff,0xff,0xff,0xff})

#endif //SERVERCLIENT_MAC_ADDR_H
