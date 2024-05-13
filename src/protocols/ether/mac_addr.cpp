#include "mac_addr.h"
#include "stdio.h"

void print_mac(mac_addr addr) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           (unsigned int) addr.octets[0],
           (unsigned int) addr.octets[1],
           (unsigned int) addr.octets[2],
           (unsigned int) addr.octets[3],
           (unsigned int) addr.octets[4],
           (unsigned int) addr.octets[5]
    );
}